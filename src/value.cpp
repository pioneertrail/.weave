#include "value.h"
#include "error_handler.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <iostream>

namespace chronovyan {

// ChronovyanArray implementation

ChronovyanArray::ChronovyanArray(std::vector<Value> elements)
    : m_elements(std::move(elements)) {}

size_t ChronovyanArray::size() const {
    return m_elements.size();
}

const Value& ChronovyanArray::at(size_t index) const {
    if (index >= m_elements.size()) {
        throw std::out_of_range("Array index out of bounds");
    }
    return m_elements[index];
}

Value& ChronovyanArray::at(size_t index) {
    if (index >= m_elements.size()) {
        throw std::out_of_range("Array index out of bounds");
    }
    return m_elements[index];
}

void ChronovyanArray::push(Value value) {
    m_elements.push_back(std::move(value));
}

// ChronovyanMap implementation

ChronovyanMap::ChronovyanMap(std::map<std::string, Value> elements)
    : m_elements(std::move(elements)) {}

size_t ChronovyanMap::size() const {
    return m_elements.size();
}

bool ChronovyanMap::contains(const std::string& key) const {
    return m_elements.find(key) != m_elements.end();
}

const Value& ChronovyanMap::at(const std::string& key) const {
    auto it = m_elements.find(key);
    if (it == m_elements.end()) {
        throw std::out_of_range("Map key not found: " + key);
    }
    return it->second;
}

Value& ChronovyanMap::at(const std::string& key) {
    auto it = m_elements.find(key);
    if (it == m_elements.end()) {
        throw std::out_of_range("Map key not found: " + key);
    }
    return it->second;
}

void ChronovyanMap::set(const std::string& key, Value value) {
    m_elements[key] = std::move(value);
}

const std::map<std::string, Value>& ChronovyanMap::getElements() const {
    return m_elements;
}

// Value implementation

Value::Value() : m_value(std::monostate()) {}

Value::Value(bool value) : m_value(value) {}

Value::Value(int64_t value) : m_value(value) {}

Value::Value(double value) : m_value(value) {}

Value::Value(std::string value) : m_value(std::move(value)) {}

Value::Value(ChronovyanArray value) 
    : m_value(std::make_shared<ChronovyanArray>(std::move(value))) {}

Value::Value(ChronovyanMap value)
    : m_value(std::make_shared<ChronovyanMap>(std::move(value))) {}

Value::Value(NativeFunction value) : m_value(std::move(value)) {}

Value::Value(ChronovyanFunction value)
    : m_value(std::make_shared<ChronovyanFunction>(std::move(value))) {}

Value::Type Value::getType() const {
    if (std::holds_alternative<std::monostate>(m_value)) {
        return Type::NIL;
    } else if (std::holds_alternative<bool>(m_value)) {
        return Type::BOOLEAN;
    } else if (std::holds_alternative<int64_t>(m_value)) {
        return Type::INTEGER;
    } else if (std::holds_alternative<double>(m_value)) {
        return Type::FLOAT;
    } else if (std::holds_alternative<std::string>(m_value)) {
        return Type::STRING;
    } else if (std::holds_alternative<std::shared_ptr<ChronovyanArray>>(m_value)) {
        return Type::ARRAY;
    } else if (std::holds_alternative<std::shared_ptr<ChronovyanMap>>(m_value)) {
        return Type::MAP;
    } else if (std::holds_alternative<NativeFunction>(m_value)) {
        return Type::NATIVE_FUNCTION;
    } else if (std::holds_alternative<std::shared_ptr<ChronovyanFunction>>(m_value)) {
        return Type::CHRONOVYAN_FUNCTION;
    }
    
    // This should never happen
    return Type::NIL;
}

bool Value::isNil() const {
    return std::holds_alternative<std::monostate>(m_value);
}

bool Value::isBoolean() const {
    return std::holds_alternative<bool>(m_value);
}

bool Value::isInteger() const {
    return std::holds_alternative<int64_t>(m_value);
}

bool Value::isFloat() const {
    return std::holds_alternative<double>(m_value);
}

bool Value::isString() const {
    return std::holds_alternative<std::string>(m_value);
}

bool Value::isArray() const {
    return std::holds_alternative<std::shared_ptr<ChronovyanArray>>(m_value);
}

bool Value::isMap() const {
    return std::holds_alternative<std::shared_ptr<ChronovyanMap>>(m_value);
}

bool Value::isNativeFunction() const {
    return std::holds_alternative<NativeFunction>(m_value);
}

bool Value::isChronovyanFunction() const {
    return std::holds_alternative<std::shared_ptr<ChronovyanFunction>>(m_value);
}

bool Value::isNumeric() const {
    return isInteger() || isFloat();
}

bool Value::asBoolean() const {
    if (isNil()) {
        return false;
    }
    
    if (isBoolean()) {
        return std::get<bool>(m_value);
    }
    
    // Any non-nil value is true
    return true;
}

int64_t Value::asInteger() const {
    if (isInteger()) {
        return std::get<int64_t>(m_value);
    }
    
    if (isFloat()) {
        return static_cast<int64_t>(std::get<double>(m_value));
    }
    
    throw std::runtime_error("Value is not an integer");
}

double Value::asFloat() const {
    if (isFloat()) {
        return std::get<double>(m_value);
    }
    
    if (isInteger()) {
        return static_cast<double>(std::get<int64_t>(m_value));
    }
    
    throw std::runtime_error("Value is not a float");
}

const std::string& Value::asString() const {
    if (!isString()) {
        throw std::runtime_error("Value is not a string");
    }
    
    return std::get<std::string>(m_value);
}

const ChronovyanArray& Value::asArray() const {
    if (!isArray()) {
        throw std::runtime_error("Value is not an array");
    }
    
    return *std::get<std::shared_ptr<ChronovyanArray>>(m_value);
}

ChronovyanArray& Value::asArray() {
    if (!isArray()) {
        throw std::runtime_error("Value is not an array");
    }
    
    return *std::get<std::shared_ptr<ChronovyanArray>>(m_value);
}

const ChronovyanMap& Value::asMap() const {
    if (!isMap()) {
        throw std::runtime_error("Value is not a map");
    }
    
    return *std::get<std::shared_ptr<ChronovyanMap>>(m_value);
}

ChronovyanMap& Value::asMap() {
    if (!isMap()) {
        throw std::runtime_error("Value is not a map");
    }
    
    return *std::get<std::shared_ptr<ChronovyanMap>>(m_value);
}

const NativeFunction& Value::asNativeFunction() const {
    if (!isNativeFunction()) {
        throw std::runtime_error("Value is not a native function");
    }
    
    return std::get<NativeFunction>(m_value);
}

const ChronovyanFunction& Value::asChronovyanFunction() const {
    if (!isChronovyanFunction()) {
        throw std::runtime_error("Value is not a Chronovyan function");
    }
    
    return *std::get<std::shared_ptr<ChronovyanFunction>>(m_value);
}

VariableModifier Value::getModifier() const {
    return m_modifier;
}

void Value::setModifier(VariableModifier modifier) {
    m_modifier = modifier;
}

const std::vector<VariableFlag>& Value::getFlags() const {
    return m_flags;
}

void Value::addFlag(VariableFlag flag) {
    // Check if the flag is already present
    for (const auto& f : m_flags) {
        if (f == flag) {
            return;
        }
    }
    
    m_flags.push_back(flag);
}

void Value::removeFlag(VariableFlag flag) {
    m_flags.erase(
        std::remove(m_flags.begin(), m_flags.end(), flag),
        m_flags.end()
    );
}

bool Value::hasFlag(VariableFlag flag) const {
    return std::find(m_flags.begin(), m_flags.end(), flag) != m_flags.end();
}

double Value::getUncertainty() const {
    return m_uncertainty;
}

void Value::setUncertainty(double uncertainty) {
    m_uncertainty = uncertainty;
}

void Value::addValueToHistory(const Value& value) {
    m_valueHistory.push_back(value);
}

const std::vector<Value>& Value::getValueHistory() const {
    return m_valueHistory;
}

void Value::setProbabilisticValue(const std::map<Value, double>& distribution) {
    m_probabilisticValue = distribution;
}

const std::map<Value, double>& Value::getProbabilisticValue() const {
    return m_probabilisticValue;
}

Value Value::resolveProbabilisticValue() const {
    // If there's no probabilistic value, return this value
    if (m_probabilisticValue.empty()) {
        return *this;
    }
    
    // Generate a random number between 0 and 1
    double r = static_cast<double>(rand()) / RAND_MAX;
    double cumulativeProbability = 0.0;
    
    // Find the value whose cumulative probability exceeds r
    for (const auto& [value, probability] : m_probabilisticValue) {
        cumulativeProbability += probability;
        if (r <= cumulativeProbability) {
            return value;
        }
    }
    
    // This should never happen if the probabilities sum to 1
    return *this;
}

std::string Value::toString() const {
    std::stringstream ss;
    
    switch (getType()) {
        case Type::NIL:
            return "nil";
            
        case Type::BOOLEAN:
            return std::get<bool>(m_value) ? "true" : "false";
            
        case Type::INTEGER:
            return std::to_string(std::get<int64_t>(m_value));
            
        case Type::FLOAT: {
            ss << std::fixed << std::setprecision(6);
            ss << std::get<double>(m_value);
            return ss.str();
        }
            
        case Type::STRING:
            return std::get<std::string>(m_value);
            
        case Type::ARRAY: {
            const auto& array = asArray();
            ss << "[";
            for (size_t i = 0; i < array.size(); ++i) {
                if (i > 0) {
                    ss << ", ";
                }
                ss << array.at(i).toString();
            }
            ss << "]";
            return ss.str();
        }
            
        case Type::MAP: {
            const auto& map = asMap();
            ss << "{";
            bool first = true;
            // Unfortunately we can't easily iterate over the map in a deterministic order
            // So we'll collect the keys, sort them, and then output in order
            std::vector<std::string> keys;
            for (const auto& pair : map.getElements()) {
                keys.push_back(pair.first);
            }
            std::sort(keys.begin(), keys.end());
            
            for (const auto& key : keys) {
                if (!first) {
                    ss << ", ";
                }
                first = false;
                ss << key << ": " << map.at(key).toString();
            }
            ss << "}";
            return ss.str();
        }
            
        case Type::NATIVE_FUNCTION:
            return "<native function>";
            
        case Type::CHRONOVYAN_FUNCTION:
            return "<function>";
    }
    
    return "<unknown>";
}

bool Value::equals(const Value& other) const {
    if (getType() != other.getType()) {
        return false;
    }
    
    switch (getType()) {
        case Type::NIL:
            return true; // All nil values are equal
            
        case Type::BOOLEAN:
            return std::get<bool>(m_value) == std::get<bool>(other.m_value);
            
        case Type::INTEGER:
            return std::get<int64_t>(m_value) == std::get<int64_t>(other.m_value);
            
        case Type::FLOAT:
            return std::get<double>(m_value) == std::get<double>(other.m_value);
            
        case Type::STRING:
            return std::get<std::string>(m_value) == std::get<std::string>(other.m_value);
            
        case Type::ARRAY: {
            const auto& thisArray = asArray();
            const auto& otherArray = other.asArray();
            
            if (thisArray.size() != otherArray.size()) {
                return false;
            }
            
            for (size_t i = 0; i < thisArray.size(); ++i) {
                if (!thisArray.at(i).equals(otherArray.at(i))) {
                    return false;
                }
            }
            
            return true;
        }
            
        case Type::MAP: {
            const auto& thisMap = asMap();
            const auto& otherMap = other.asMap();
            
            if (thisMap.size() != otherMap.size()) {
                return false;
            }
            
            // Check that every key in thisMap exists in otherMap and has the same value
            for (const auto& pair : thisMap.getElements()) {
                if (!otherMap.contains(pair.first) || !pair.second.equals(otherMap.at(pair.first))) {
                    return false;
                }
            }
            
            return true;
        }
            
        case Type::NATIVE_FUNCTION:
        case Type::CHRONOVYAN_FUNCTION:
            // Functions are only equal if they are the same object
            return &m_value == &other.m_value;
    }
    
    return false;
}

// Utility functions

bool areEqual(const Value& a, const Value& b) {
    return a.equals(b);
}

Value add(const Value& a, const Value& b) {
    if (a.isString() || b.isString()) {
        // String concatenation
        std::string result = a.toString() + b.toString();
        return Value(result);
    }
    
    if (a.isNumeric() && b.isNumeric()) {
        if (a.isInteger() && b.isInteger()) {
            return Value(a.asInteger() + b.asInteger());
        } else {
            return Value(a.asFloat() + b.asFloat());
        }
    }
    
    throw std::runtime_error("Cannot add values of these types");
}

Value subtract(const Value& a, const Value& b) {
    if (a.isNumeric() && b.isNumeric()) {
        if (a.isInteger() && b.isInteger()) {
            return Value(a.asInteger() - b.asInteger());
        } else {
            return Value(a.asFloat() - b.asFloat());
        }
    }
    
    throw std::runtime_error("Cannot subtract values of these types");
}

Value multiply(const Value& a, const Value& b) {
    if (a.isNumeric() && b.isNumeric()) {
        if (a.isInteger() && b.isInteger()) {
            return Value(a.asInteger() * b.asInteger());
        } else {
            return Value(a.asFloat() * b.asFloat());
        }
    }
    
    throw std::runtime_error("Cannot multiply values of these types");
}

Value divide(const Value& a, const Value& b) {
    if (a.isNumeric() && b.isNumeric()) {
        if (b.isInteger() && b.asInteger() == 0) {
            throw std::runtime_error("Division by zero");
        }
        
        if (b.isFloat() && b.asFloat() == 0.0) {
            throw std::runtime_error("Division by zero");
        }
        
        // Always return a float for division
        return Value(a.asFloat() / b.asFloat());
    }
    
    throw std::runtime_error("Cannot divide values of these types");
}

Value modulo(const Value& a, const Value& b) {
    if (a.isInteger() && b.isInteger()) {
        if (b.asInteger() == 0) {
            throw std::runtime_error("Modulo by zero");
        }
        
        return Value(a.asInteger() % b.asInteger());
    }
    
    if (a.isNumeric() && b.isNumeric()) {
        if (b.asFloat() == 0.0) {
            throw std::runtime_error("Modulo by zero");
        }
        
        return Value(std::fmod(a.asFloat(), b.asFloat()));
    }
    
    throw std::runtime_error("Cannot compute modulo of these types");
}

Value negate(const Value& a) {
    if (a.isInteger()) {
        return Value(-a.asInteger());
    }
    
    if (a.isFloat()) {
        return Value(-a.asFloat());
    }
    
    throw std::runtime_error("Cannot negate a non-numeric value");
}

Value logicalNot(const Value& a) {
    return Value(!a.asBoolean());
}

} // namespace chronovyan 