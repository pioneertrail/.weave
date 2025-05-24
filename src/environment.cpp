#include "environment.h"
#include "error_handler.h"
#include <sstream>

namespace chronovyan {

Environment::Environment() : m_enclosing(nullptr) {}

Environment::Environment(std::shared_ptr<Environment> enclosing) 
    : m_enclosing(std::move(enclosing)) {}

void Environment::define(const std::string& name, Value value) {
    m_values[name] = std::move(value);
}

Value Environment::get(const std::string& name) const {
    auto it = m_values.find(name);
    if (it != m_values.end()) {
        return it->second;
    }
    
    // If the variable isn't found in this environment, check the enclosing one
    if (m_enclosing) {
        return m_enclosing->get(name);
    }
    
    std::stringstream ss;
    ss << "Undefined variable '" << name << "'";
    throw ChronovyanRuntimeError(ss.str(), SourceLocation());
}

void Environment::assign(const std::string& name, Value value) {
    auto it = m_values.find(name);
    if (it != m_values.end()) {
        // Check if the variable is STATIC and already has a value
        if (it->second.hasFlag(VariableFlag::STATIC) && !it->second.isNil()) {
            std::stringstream ss;
            ss << "Cannot assign to STATIC variable '" << name << "'";
            throw ChronovyanRuntimeError(ss.str(), SourceLocation());
        }
        
        // If variable has ECHO flag, add the current value to history
        if (it->second.hasFlag(VariableFlag::ECHO)) {
            value.addValueToHistory(it->second);
        }
        
        // Update the variable
        it->second = std::move(value);
        return;
    }
    
    // If the variable isn't found in this environment, check the enclosing one
    if (m_enclosing) {
        m_enclosing->assign(name, std::move(value));
        return;
    }
    
    std::stringstream ss;
    ss << "Undefined variable '" << name << "'";
    throw ChronovyanRuntimeError(ss.str(), SourceLocation());
}

bool Environment::contains(const std::string& name) const {
    return m_values.find(name) != m_values.end() || 
           (m_enclosing && m_enclosing->contains(name));
}

std::shared_ptr<Environment> Environment::getEnclosing() const {
    return m_enclosing;
}

} // namespace chronovyan 