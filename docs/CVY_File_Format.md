# Chronovyan .cvy File Format Reference

## Overview

The `.cvy` file format is the standard file extension for Chronovyan language source code. This document provides a technical reference for the format, including structure, content, and best practices.

## File Characteristics

| Characteristic | Description |
|----------------|-------------|
| Extension      | `.cvy` |
| Format         | Plain text (UTF-8 encoded) |
| Line Endings   | Platform-independent (LF or CRLF) |
| Comment Style  | `// Single line comment` or `/* Multi-line comment */` |

## Basic Structure

A typical `.cvy` file follows this general structure:

```
// File header comment describing purpose
// Author information, version, etc.

// Import statements (if applicable)
IMPORT "standard_library.cvy";

// Constants and global variables
CONF INT MAX_ITERATIONS = 100;
REB FLOAT INSTABILITY_FACTOR = 0.5;

// Function declarations
FUNC INT calculate_temporal_variance(INT base_time, FLOAT modifier) {
    // Function body
    RETURN base_time * modifier;
}

// Main program logic
FUNC VOID main() {
    // Program statements
    PRINT("Executing temporal sequence");
    
    // Temporal operations
    FOR_CHRONON(CONF INT i = 0; i < MAX_ITERATIONS; i++) {
        // Loop body
    }
}

// Call main function
main();
```

## Supported Content Types

Chronovyan `.cvy` files can contain:

1. **Variable Declarations** - Defining CONF and REB variables
2. **Function Definitions** - Declaring reusable code blocks
3. **Control Flow Statements** - Conditionals and loops
4. **Temporal Operations** - Timeline manipulations
5. **Resource Management** - Chronon and Aethel usage
6. **Comments** - Code documentation

## Sample Declarations

### Variables

```
// Basic variable declarations
CONF INT stability_factor = 42;
REB STRING message = "Temporal flux detected";

// With modifiers
CONF::ANCHOR FLOAT anchor_point = 3.14159;
REB::WEAVER BOOL is_divergent = true;
```

### Functions

```
// Simple function
FUNC VOID greet(STRING name) {
    PRINT("Hello, " + name + "!");
}

// Function with return value
FUNC INT calculate_paradox(INT actions, FLOAT intensity) {
    RETURN actions * intensity;
}
```

### Temporal Operations

```
// Creating time points
TIME_POINT save_point = CREATE_TIME_POINT("checkpoint");

// Branching timelines
BRANCH_TIMELINE("alternate") {
    // Timeline-specific code
}

// Rewinding flow
REWIND_FLOW(save_point);
```

## Best Practices

1. **File Naming** - Use lowercase names with underscores for clarity (`temporal_simulation.cvy`)
2. **Comments** - Include a header comment with description and author information
3. **Structure** - Organize code in a logical order: imports, constants, functions, main logic
4. **Indentation** - Use consistent indentation (typically 4 spaces or 1 tab)
5. **Resource Usage** - Include resource consumption indicators for computationally expensive operations

## Integration with Other Chronovyan File Types

The `.cvy` extension is the primary source code format, but it integrates with other Chronovyan file types:

- `.tcvy` - Timeline definition files
- `.ancvy` - Compiled anchor files
- `.flxcvy` - Compiled flux files
- `.shdcvy` - Temporal shard definitions
- `.infcvy` - Weaver schema specifications

## Encoding Considerations

- Always save `.cvy` files as UTF-8 to ensure compatibility
- Avoid Byte Order Marks (BOM) in source files
- Non-ASCII characters are supported in strings and comments

## Tools Support

Most text editors can be configured to recognize `.cvy` files. Syntax highlighting packages are available for:

- Visual Studio Code
- Sublime Text
- Vim
- Emacs

## Version Compatibility

The `.cvy` format is designed to be backward compatible. Files created for earlier versions of Chronovyan should continue to work with newer interpreters, although new features may not be available.

## Example

```chronovyan
// Temporal simulation example
import temporal::core;

timeline main() {
    // Initialize simulation parameters
    var<anchor> simulation_steps = 100;
    var<flux> stability_threshold = 0.75;
    
    echo("Starting temporal simulation...");
    // More code here...
    
    return stability;
}
``` 