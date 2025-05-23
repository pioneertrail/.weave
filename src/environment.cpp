#include "environment.h"
#include "error_handler.h"

namespace chronovyan {

Environment::Environment()
    : m_enclosing(nullptr)
{
    // Initialize global environment
}

Environment::Environment(std::shared_ptr<Environment> enclosing)
    : m_enclosing(std::move(enclosing))
{
    // Initialize local environment with enclosing scope
}

void Environment::define(const std::string& name, Value value) {
    // Define a new variable or update existing variable in current scope
    m_values[name] = std::move(value);
}

Value Environment::get(const std::string& name) const {
    // Look up variable in current scope
    auto it = m_values.find(name);
    if (it != m_values.end()) {
        return it->second;
    }
    
    // If not found and we have an enclosing environment, look there
    if (m_enclosing) {
        return m_enclosing->get(name);
    }
    
    // Not found in any enclosing scope
    throw ChronovyanRuntimeError(
        "Undefined variable '" + name + "'",
        SourceLocation()
    );
}

void Environment::assign(const std::string& name, Value value) {
    // Try to assign in current scope
    auto it = m_values.find(name);
    if (it != m_values.end()) {
        // Check for STATIC flag - cannot reassign static variables
        if (it->second.hasFlag(VariableFlag::STATIC)) {
            throw ChronovyanRuntimeError(
                "Cannot reassign static variable '" + name + "'",
                SourceLocation()
            );
        }
        
        it->second = std::move(value);
        return;
    }
    
    // If not found and we have an enclosing environment, try there
    if (m_enclosing) {
        m_enclosing->assign(name, std::move(value));
        return;
    }
    
    // Not found in any enclosing scope
    throw ChronovyanRuntimeError(
        "Cannot assign to undefined variable '" + name + "'",
        SourceLocation()
    );
}

bool Environment::contains(const std::string& name) const {
    return m_values.find(name) != m_values.end();
}

std::shared_ptr<Environment> Environment::getEnvironmentWhere(const std::string& name) const {
    // Check if the variable is in this environment
    if (contains(name)) {
        return const_cast<Environment*>(this)->shared_from_this();
    }
    
    // If not, check the enclosing environment
    if (m_enclosing) {
        return m_enclosing->getEnvironmentWhere(name);
    }
    
    // Not found anywhere
    return nullptr;
}

std::optional<std::reference_wrapper<Value>> Environment::getReference(const std::string& name) {
    // Look up variable in current scope
    auto it = m_values.find(name);
    if (it != m_values.end()) {
        return std::optional<std::reference_wrapper<Value>>(it->second);
    }
    
    // If not found and we have an enclosing environment, look there
    if (m_enclosing) {
        return m_enclosing->getReference(name);
    }
    
    // Not found in any enclosing scope
    return std::nullopt;
}

std::shared_ptr<Environment> Environment::getEnclosing() const {
    return m_enclosing;
}

std::shared_ptr<Environment> Environment::clone() const {
    // Create a new environment with the same enclosing environment
    auto cloned = std::make_shared<Environment>(m_enclosing);
    
    // Copy all values
    for (const auto& [name, value] : m_values) {
        cloned->m_values[name] = value;
    }
    
    return cloned;
}

} // namespace chronovyan 