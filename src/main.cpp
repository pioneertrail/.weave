#include "source_file.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "error_handler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <stdexcept>
#include <algorithm>

using namespace chronovyan;

// Function prototypes
void runFile(const std::string& path);
void runRepl();
void runString(const std::string& source, const std::string& sourceName = "<string>");
void printHelp();
bool hasValidExtension(const std::string& filename);

int main(int argc, char* argv[]) {
    try {
        if (argc == 1) {
            // No arguments, run REPL
            runRepl();
        } else if (argc == 2) {
            std::string arg = argv[1];
            if (arg == "--help" || arg == "-h") {
                printHelp();
            } else {
                // Check if file has correct extension
                if (!hasValidExtension(arg)) {
                    std::cerr << "Warning: File does not have a .cvy extension. It may not be a valid Chronovyan script." << std::endl;
                    std::cerr << "Do you want to continue? (y/n): ";
                    char response;
                    std::cin >> response;
                    if (response != 'y' && response != 'Y') {
                        return 0;
                    }
                }
                // Run file
                runFile(arg);
            }
        } else {
            std::cerr << "Usage: chronovyan [script]" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

bool hasValidExtension(const std::string& filename) {
    if (filename.size() < 5) { // At minimum: "x.cvy"
        return false;
    }
    
    // Convert filename to lowercase for case-insensitive comparison
    std::string lowerFilename = filename;
    std::transform(lowerFilename.begin(), lowerFilename.end(), lowerFilename.begin(), 
                 [](unsigned char c){ return std::tolower(c); });
    
    // Check for .cvy extension
    return lowerFilename.substr(lowerFilename.size() - 4) == ".cvy";
}

void runFile(const std::string& path) {
    try {
        // Create a source file from the file path
        auto sourceFile = std::make_shared<SourceFile>(path);
        
        // Run the source
        runString(sourceFile->getSource(), sourceFile->getName());
        
        // Check for errors
        auto& errorHandler = ErrorHandler::getInstance();
        if (errorHandler.hasErrors()) {
            // Report errors
            for (const auto& error : errorHandler.getErrors()) {
                std::cerr << error.toString() << std::endl;
            }
            
            // Exit with error code
            std::exit(65);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading file: " << e.what() << std::endl;
        std::exit(74);
    }
}

void runRepl() {
    // Create an interpreter that will persist between lines
    Interpreter interpreter;
    
    std::cout << "Chronovyan Language Interpreter (REPL)" << std::endl;
    std::cout << "Type 'exit' to quit, 'help' for help." << std::endl;
    
    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            break;
        }
        
        // Check for special commands
        if (line == "exit") {
            break;
        } else if (line == "help") {
            std::cout << "REPL Commands:" << std::endl;
            std::cout << "  exit     - Exit the REPL" << std::endl;
            std::cout << "  help     - Display this help message" << std::endl;
            std::cout << "  clear    - Clear the screen" << std::endl;
            std::cout << "  reset    - Reset the interpreter state" << std::endl;
            std::cout << "  paradox  - Show the current paradox level" << std::endl;
            std::cout << "  resources - Show the current resource levels" << std::endl;
            continue;
        } else if (line == "clear") {
#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
            continue;
        } else if (line == "reset") {
            interpreter = Interpreter();
            std::cout << "Interpreter state reset." << std::endl;
            continue;
        } else if (line == "paradox") {
            std::cout << "Current PARADOX_LEVEL: " << interpreter.getRuntime()->getParadoxLevel() << std::endl;
            continue;
        } else if (line == "resources") {
            std::cout << "Aethel: " << interpreter.getRuntime()->getAethelLevel() << std::endl;
            std::cout << "Chronons: " << interpreter.getRuntime()->getChrononsLevel() << std::endl;
            continue;
        }
        
        try {
            // Create a source file from the line
            auto sourceFile = std::make_shared<SourceFile>(std::move(line), "<repl>");
            
            // Create a lexer
            auto lexer = std::make_shared<Lexer>(sourceFile);
            
            // Create a parser
            auto parser = std::make_shared<Parser>(lexer);
            
            // Parse the input
            auto program = parser->parse();
            
            // Check for parse errors
            auto& errorHandler = ErrorHandler::getInstance();
            if (errorHandler.hasErrors()) {
                // Report errors
                for (const auto& error : errorHandler.getErrors()) {
                    std::cerr << error.toString() << std::endl;
                }
                
                // Clear errors for next line
                errorHandler.clearErrors();
                continue;
            }
            
            // Interpret the program
            Value result = interpreter.interpret(*program);
            
            // Print the result
            std::cout << result.toString() << std::endl;
        } catch (const ChronovyanException& e) {
            std::cerr << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        
        // Clear errors for next line
        ErrorHandler::getInstance().clearErrors();
    }
}

void runString(const std::string& source, const std::string& sourceName) {
    try {
        // Create a source file from the string
        auto sourceFile = std::make_shared<SourceFile>(std::string(source), sourceName);
        
        // Create a lexer
        auto lexer = std::make_shared<Lexer>(sourceFile);
        
        // Create a parser
        auto parser = std::make_shared<Parser>(lexer);
        
        // Parse the source
        auto program = parser->parse();
        
        // Check for parse errors
        auto& errorHandler = ErrorHandler::getInstance();
        if (errorHandler.hasErrors()) {
            return;  // Don't try to interpret if there were parse errors
        }
        
        // Create an interpreter
        Interpreter interpreter;
        
        // Interpret the program
        interpreter.interpret(*program);
    } catch (const ChronovyanException& e) {
        std::cerr << e.what() << std::endl;
        ErrorHandler::getInstance().reportError(SourceLocation(), e.what());
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        ErrorHandler::getInstance().reportError(SourceLocation(), std::string("Error: ") + e.what());
    }
}

void printHelp() {
    std::cout << "Chronovyan Language Interpreter" << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  chronovyan              Start the REPL" << std::endl;
    std::cout << "  chronovyan <file.cvy>   Run a Chronovyan script (.cvy file)" << std::endl;
    std::cout << "  chronovyan --help       Display this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "In the REPL, type 'help' for REPL-specific commands." << std::endl;
} 