#ifndef CHRONOVYAN_VALUE_H
#define CHRONOVYAN_VALUE_H

#include "ast_nodes.h"
#include <variant>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <functional>

namespace chronovyan {

// Forward declarations
class Value;
class Environment;

/**
 * @brief Type alias for a function implemented in C++
 */
using NativeFunction = std::function<Value(const std::vector<Value>&)>;

/**
 * @class ChronovyanFunction
 * @brief Represents a function defined in Chronovyan
 */
class ChronovyanFunction {
public:
    // Function definition would go here
    // This is a placeholder for now
};

/**
 * @class ChronovyanArray
 * @brief Represents an array of values
 */
class ChronovyanArray {
public:
    explicit ChronovyanArray(std::vector<Value> elements);
    
    size_t size() const;
    const Value& at(size_t index) const;
    Value& at(size_t index);
    void push(Value value);
    
private:
    std::vector<Value> m_elements;
};

/**
 * @class ChronovyanMap
 * @brief Represents a map of values
 */
class ChronovyanMap {
public:
    explicit ChronovyanMap(std::map<std::string, Value> elements = {});
    
    size_t size() const;
    bool contains(const std::string& key) const;
    const Value& at(const std::string& key) const;
    Value& at(const std::string& key);
    void set(const std::string& key, Value value);
    
private:
    std::map<std::string, Value> m_elements;
};

/**
 * @class Value
 * @brief Represents a runtime value in Chronovyan
 */
class Value {
public:
    /**
     * @brief The possible types of values
     */
    enum class Type {
        NIL,
        BOOLEAN,
        INTEGER,
        FLOAT,
        STRING,
        ARRAY,
        MAP,
        NATIVE_FUNCTION,
        CHRONOVYAN_FUNCTION
    };
    
    // Constructors for different value types
    Value();  // Nil value
    explicit Value(bool value);
    explicit Value(int64_t value);
    explicit Value(double value);
    explicit Value(std::string value);
    explicit Value(ChronovyanArray value);
    explicit Value(ChronovyanMap value);
    explicit Value(NativeFunction value);
    explicit Value(ChronovyanFunction value);
    
    // Type checking
    Type getType() const;
    bool isNil() const;
    bool isBoolean() const;
    bool isInteger() const;
    bool isFloat() const;
    bool isString() const;
    bool isArray() const;
    bool isMap() const;
    bool isNativeFunction() const;
    bool isChronovyanFunction() const;
    bool isNumeric() const;
    
    // Value getters
    bool asBoolean() const;
    int64_t asInteger() const;
    double asFloat() const;
    const std::string& asString() const;
    const ChronovyanArray& asArray() const;
    ChronovyanArray& asArray();
    const ChronovyanMap& asMap() const;
    ChronovyanMap& asMap();
    const NativeFunction& asNativeFunction() const;
    const ChronovyanFunction& asChronovyanFunction() const;
    
    // Variable properties
    VariableModifier getModifier() const;
    void setModifier(VariableModifier modifier);
    
    const std::vector<VariableFlag>& getFlags() const;
    void addFlag(VariableFlag flag);
    void removeFlag(VariableFlag flag);
    bool hasFlag(VariableFlag flag) const;
    
    // For REB variables
    double getUncertainty() const;
    void setUncertainty(double uncertainty);
    
    // For variables with ECHO flag
    void addValueToHistory(const Value& value);
    const std::vector<Value>& getValueHistory() const;
    
    // For WEAVER flag
    void setProbabilisticValue(const std::map<Value, double>& distribution);
    const std::map<Value, double>& getProbabilisticValue() const;
    Value resolveProbabilisticValue() const;
    
    // Utility methods
    std::string toString() const;
    bool equals(const Value& other) const;
    
private:
    using ValueVariant = std::variant<
        std::monostate,                // NIL
        bool,                         // BOOLEAN
        int64_t,                      // INTEGER
        double,                       // FLOAT
        std::string,                  // STRING
        std::shared_ptr<ChronovyanArray>,  // ARRAY
        std::shared_ptr<ChronovyanMap>,    // MAP
        NativeFunction,               // NATIVE_FUNCTION
        std::shared_ptr<ChronovyanFunction> // CHRONOVYAN_FUNCTION
    >;
    
    ValueVariant m_value;
    VariableModifier m_modifier = VariableModifier::CONF;
    std::vector<VariableFlag> m_flags;
    
    // For REB variables
    double m_uncertainty = 0.0;
    
    // For variables with ECHO flag
    std::vector<Value> m_valueHistory;
    
    // For WEAVER flag (quantum probabilistic state)
    std::map<Value, double> m_probabilisticValue;
};

// Utility functions
bool areEqual(const Value& a, const Value& b);
Value add(const Value& a, const Value& b);
Value subtract(const Value& a, const Value& b);
Value multiply(const Value& a, const Value& b);
Value divide(const Value& a, const Value& b);
Value modulo(const Value& a, const Value& b);
Value negate(const Value& a);
Value logicalNot(const Value& a);

} // namespace chronovyan

#endif // CHRONOVYAN_VALUE_H 