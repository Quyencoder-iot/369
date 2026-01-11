/**
 * @file json_parser.hpp
 * @brief Simple JSON parser cho vehicle database
 * 
 * Không sử dụng thư viện ngoài - chỉ dùng STL
 * Hỗ trợ: objects, arrays, strings, numbers, booleans
 */

#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <algorithm>

namespace json {

enum class ValueType {
    Null,
    Boolean,
    Number,
    String,
    Array,
    Object
};

class Value;
using Object = std::map<std::string, std::shared_ptr<Value>>;
using Array = std::vector<std::shared_ptr<Value>>;

/**
 * @brief JSON Value - có thể là bất kỳ kiểu JSON nào
 */
class Value {
public:
    Value() : type_(ValueType::Null) {}
    Value(bool b) : type_(ValueType::Boolean), bool_val_(b) {}
    Value(double d) : type_(ValueType::Number), num_val_(d) {}
    Value(const std::string& s) : type_(ValueType::String), str_val_(s) {}
    Value(const Object& o) : type_(ValueType::Object), obj_val_(o) {}
    Value(const Array& a) : type_(ValueType::Array), arr_val_(a) {}
    
    ValueType Type() const { return type_; }
    
    bool AsBool() const { return bool_val_; }
    double AsNumber() const { return num_val_; }
    int AsInt() const { return static_cast<int>(num_val_); }
    float AsFloat() const { return static_cast<float>(num_val_); }
    const std::string& AsString() const { return str_val_; }
    const Object& AsObject() const { return obj_val_; }
    const Array& AsArray() const { return arr_val_; }
    
    // Access nested values
    std::shared_ptr<Value> Get(const std::string& key) const {
        if (type_ != ValueType::Object) return nullptr;
        auto it = obj_val_.find(key);
        return (it != obj_val_.end()) ? it->second : nullptr;
    }
    
    std::shared_ptr<Value> Get(size_t index) const {
        if (type_ != ValueType::Array || index >= arr_val_.size()) return nullptr;
        return arr_val_[index];
    }
    
    // Operator[] for convenience
    std::shared_ptr<Value> operator[](const std::string& key) const {
        return Get(key);
    }
    
    std::shared_ptr<Value> operator[](size_t index) const {
        return Get(index);
    }
    
    // Check if key exists
    bool Has(const std::string& key) const {
        return type_ == ValueType::Object && obj_val_.find(key) != obj_val_.end();
    }
    
    // Get array size
    size_t Size() const {
        if (type_ == ValueType::Array) return arr_val_.size();
        if (type_ == ValueType::Object) return obj_val_.size();
        return 0;
    }
    
private:
    ValueType type_;
    bool bool_val_ = false;
    double num_val_ = 0;
    std::string str_val_;
    Object obj_val_;
    Array arr_val_;
};

/**
 * @brief JSON Parser class
 */
class Parser {
public:
    std::shared_ptr<Value> Parse(const std::string& json_str) {
        pos_ = 0;
        input_ = json_str;
        return ParseValue();
    }
    
    std::shared_ptr<Value> ParseFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return Parse(buffer.str());
    }
    
private:
    void SkipWhitespace() {
        while (pos_ < input_.size() && 
               (input_[pos_] == ' ' || input_[pos_] == '\n' || 
                input_[pos_] == '\r' || input_[pos_] == '\t')) {
            pos_++;
        }
    }
    
    char Peek() {
        SkipWhitespace();
        return (pos_ < input_.size()) ? input_[pos_] : '\0';
    }
    
    char Get() {
        SkipWhitespace();
        return (pos_ < input_.size()) ? input_[pos_++] : '\0';
    }
    
    std::shared_ptr<Value> ParseValue() {
        char c = Peek();
        if (c == '{') return ParseObject();
        if (c == '[') return ParseArray();
        if (c == '"') return ParseString();
        if (c == 't' || c == 'f') return ParseBool();
        if (c == 'n') return ParseNull();
        if (c == '-' || (c >= '0' && c <= '9')) return ParseNumber();
        throw std::runtime_error("Unexpected character: " + std::string(1, c));
    }
    
    std::shared_ptr<Value> ParseObject() {
        Get(); // consume '{'
        Object obj;
        
        if (Peek() == '}') {
            Get();
            return std::make_shared<Value>(obj);
        }
        
        while (true) {
            // Parse key
            std::string key = ParseString()->AsString();
            
            // Expect ':'
            if (Get() != ':') throw std::runtime_error("Expected ':'");
            
            // Parse value
            obj[key] = ParseValue();
            
            // Check for ',' or '}'
            char c = Get();
            if (c == '}') break;
            if (c != ',') throw std::runtime_error("Expected ',' or '}'");
        }
        
        return std::make_shared<Value>(obj);
    }
    
    std::shared_ptr<Value> ParseArray() {
        Get(); // consume '['
        Array arr;
        
        if (Peek() == ']') {
            Get();
            return std::make_shared<Value>(arr);
        }
        
        while (true) {
            arr.push_back(ParseValue());
            
            char c = Get();
            if (c == ']') break;
            if (c != ',') throw std::runtime_error("Expected ',' or ']'");
        }
        
        return std::make_shared<Value>(arr);
    }
    
    std::shared_ptr<Value> ParseString() {
        Get(); // consume opening '"'
        std::string str;
        
        while (pos_ < input_.size() && input_[pos_] != '"') {
            if (input_[pos_] == '\\' && pos_ + 1 < input_.size()) {
                pos_++;
                switch (input_[pos_]) {
                    case 'n': str += '\n'; break;
                    case 't': str += '\t'; break;
                    case 'r': str += '\r'; break;
                    case '"': str += '"'; break;
                    case '\\': str += '\\'; break;
                    default: str += input_[pos_];
                }
            } else {
                str += input_[pos_];
            }
            pos_++;
        }
        
        if (pos_ < input_.size()) pos_++; // consume closing '"'
        return std::make_shared<Value>(str);
    }
    
    std::shared_ptr<Value> ParseNumber() {
        size_t start = pos_;
        if (input_[pos_] == '-') pos_++;
        
        while (pos_ < input_.size() && 
               (input_[pos_] >= '0' && input_[pos_] <= '9')) {
            pos_++;
        }
        
        if (pos_ < input_.size() && input_[pos_] == '.') {
            pos_++;
            while (pos_ < input_.size() && 
                   (input_[pos_] >= '0' && input_[pos_] <= '9')) {
                pos_++;
            }
        }
        
        // Handle scientific notation
        if (pos_ < input_.size() && (input_[pos_] == 'e' || input_[pos_] == 'E')) {
            pos_++;
            if (input_[pos_] == '+' || input_[pos_] == '-') pos_++;
            while (pos_ < input_.size() && 
                   (input_[pos_] >= '0' && input_[pos_] <= '9')) {
                pos_++;
            }
        }
        
        return std::make_shared<Value>(std::stod(input_.substr(start, pos_ - start)));
    }
    
    std::shared_ptr<Value> ParseBool() {
        if (input_.substr(pos_, 4) == "true") {
            pos_ += 4;
            return std::make_shared<Value>(true);
        }
        if (input_.substr(pos_, 5) == "false") {
            pos_ += 5;
            return std::make_shared<Value>(false);
        }
        throw std::runtime_error("Invalid boolean");
    }
    
    std::shared_ptr<Value> ParseNull() {
        if (input_.substr(pos_, 4) == "null") {
            pos_ += 4;
            return std::make_shared<Value>();
        }
        throw std::runtime_error("Invalid null");
    }
    
    std::string input_;
    size_t pos_ = 0;
};

} // namespace json

#endif // JSON_PARSER_HPP
