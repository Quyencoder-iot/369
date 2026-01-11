// =============================================
// MODULE: ara::log::Logger
// ---------------------------------------------
// Mục đích: Logging theo chuẩn AUTOSAR
// Giải thích:
// - Context ID để phân loại log
// - Multiple log levels
// - Thread-safe với mutex
// =============================================

#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <fstream>

namespace ara {
namespace log {

enum class LogLevel {
    kOff = 0,
    kFatal,
    kError,
    kWarn,
    kInfo,
    kDebug,
    kVerbose
};

class Logger {
public:
    Logger(const std::string& ctxId, const std::string& ctxDesc)
        : contextId_(ctxId)
        , contextDesc_(ctxDesc)
        , minLevel_(LogLevel::kInfo)
        , enableConsole_(true)
        , enableFile_(false)
    {}
    
    static Logger CreateLogger(const std::string& ctxId, 
                               const std::string& ctxDesc) {
        return Logger(ctxId, ctxDesc);
    }
    
    void SetLogLevel(LogLevel level) {
        minLevel_ = level;
    }
    
    void EnableConsole(bool enable) {
        enableConsole_ = enable;
    }
    
    void EnableFile(bool enable, const std::string& path = "") {
        enableFile_ = enable;
        if (enable && !path.empty()) {
            logFile_.open(path, std::ios::app);
        }
    }
    
    void LogFatal(const std::string& msg) {
        log(LogLevel::kFatal, msg);
    }
    
    void LogError(const std::string& msg) {
        log(LogLevel::kError, msg);
    }
    
    void LogWarn(const std::string& msg) {
        log(LogLevel::kWarn, msg);
    }
    
    void LogInfo(const std::string& msg) {
        log(LogLevel::kInfo, msg);
    }
    
    void LogDebug(const std::string& msg) {
        log(LogLevel::kDebug, msg);
    }
    
    void LogVerbose(const std::string& msg) {
        log(LogLevel::kVerbose, msg);
    }
    
    // Stream-style logging
    class LogStream {
    public:
        LogStream(Logger& logger, LogLevel level) 
            : logger_(logger), level_(level) {}
        
        ~LogStream() {
            logger_.log(level_, stream_.str());
        }
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            stream_ << value;
            return *this;
        }
        
    private:
        Logger& logger_;
        LogLevel level_;
        std::stringstream stream_;
    };
    
    LogStream operator()(LogLevel level) {
        return LogStream(*this, level);
    }

private:
    void log(LogLevel level, const std::string& msg) {
        if (level > minLevel_) return;
        
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        
        std::string formattedMsg = formatMessage(level, msg);
        
        if (enableConsole_) {
            std::cout << formattedMsg << std::endl;
        }
        
        if (enableFile_ && logFile_.is_open()) {
            logFile_ << formattedMsg << std::endl;
            logFile_.flush();
        }
    }
    
    std::string formatMessage(LogLevel level, const std::string& msg) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S")
           << "." << std::setfill('0') << std::setw(3) << ms.count();
        
        std::stringstream result;
        result << "[" << ss.str() << "]"
               << "[" << levelToString(level) << "]"
               << "[" << contextId_ << "] "
               << msg;
        
        return result.str();
    }
    
    static const char* levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::kFatal:   return "\033[31mFATAL\033[0m";
            case LogLevel::kError:   return "\033[31mERROR\033[0m";
            case LogLevel::kWarn:    return "\033[33mWARN \033[0m";
            case LogLevel::kInfo:    return "\033[32mINFO \033[0m";
            case LogLevel::kDebug:   return "\033[36mDEBUG\033[0m";
            case LogLevel::kVerbose: return "\033[37mVERB \033[0m";
            default:                 return "?????";
        }
    }
    
    std::string contextId_;
    std::string contextDesc_;
    LogLevel minLevel_;
    bool enableConsole_;
    bool enableFile_;
    std::ofstream logFile_;
};

// Macro helpers
#define ARA_LOG_INFO(logger, msg) logger.LogInfo(msg)
#define ARA_LOG_WARN(logger, msg) logger.LogWarn(msg)
#define ARA_LOG_ERROR(logger, msg) logger.LogError(msg)
#define ARA_LOG_DEBUG(logger, msg) logger.LogDebug(msg)

}  // namespace log
}  // namespace ara
