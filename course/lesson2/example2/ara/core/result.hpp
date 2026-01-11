// =============================================
// MODULE: ara::core::Result
// ---------------------------------------------
// Mục đích: Error handling không dùng exception
// Giải thích:
// - Result chứa hoặc value hoặc error
// - Pattern phổ biến trong Adaptive AUTOSAR
// =============================================

#pragma once
#include <variant>
#include <string>
#include <optional>

namespace ara {
namespace core {

class ErrorCode {
public:
    ErrorCode() : value_(0) {}
    ErrorCode(int32_t value, const std::string& msg = "") 
        : value_(value), message_(msg) {}
    
    int32_t Value() const { return value_; }
    const std::string& Message() const { return message_; }
    
    explicit operator bool() const { return value_ != 0; }
    
    bool operator==(const ErrorCode& other) const {
        return value_ == other.value_;
    }
    
    bool operator!=(const ErrorCode& other) const {
        return !(*this == other);
    }

private:
    int32_t value_;
    std::string message_;
};

// Common error codes
namespace errc {
    inline ErrorCode success() { return ErrorCode(0, "Success"); }
    inline ErrorCode invalid_argument() { return ErrorCode(1, "Invalid argument"); }
    inline ErrorCode not_found() { return ErrorCode(2, "Not found"); }
    inline ErrorCode timeout() { return ErrorCode(3, "Timeout"); }
    inline ErrorCode connection_error() { return ErrorCode(4, "Connection error"); }
}

template<typename T, typename E = ErrorCode>
class Result {
public:
    // Constructors
    Result(const T& value) : data_(value) {}
    Result(T&& value) : data_(std::move(value)) {}
    Result(const E& error) : data_(error) {}
    Result(E&& error) : data_(std::move(error)) {}
    
    // Factory methods
    static Result FromValue(T value) { 
        return Result(std::move(value)); 
    }
    
    static Result FromError(E error) { 
        return Result(std::move(error)); 
    }
    
    // Accessors
    bool HasValue() const { 
        return std::holds_alternative<T>(data_); 
    }
    
    const T& Value() const { 
        return std::get<T>(data_); 
    }
    
    T& Value() { 
        return std::get<T>(data_); 
    }
    
    const E& Error() const { 
        return std::get<E>(data_); 
    }
    
    // Value or default
    T ValueOr(T default_value) const {
        if (HasValue()) {
            return Value();
        }
        return default_value;
    }
    
    // Operators
    explicit operator bool() const { 
        return HasValue(); 
    }
    
    const T& operator*() const { 
        return Value(); 
    }
    
    T& operator*() { 
        return Value(); 
    }
    
    const T* operator->() const { 
        return &Value(); 
    }
    
    T* operator->() { 
        return &Value(); 
    }

private:
    std::variant<T, E> data_;
};

}  // namespace core
}  // namespace ara
