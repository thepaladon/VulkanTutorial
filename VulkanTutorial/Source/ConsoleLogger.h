#pragma once

// This entire logger is written with ChatGPT 3.5 :p
// ...aaaaand a bit of manual debugging 

namespace Logger {

    enum class MessageType {
        INFO,
        DEBUG,
        WARNING,
        ERROR
    };

    void logInfo(const char* format, ...) ;
    void logDebug(const char* format, ...);
    void logWarning(const char* format, ...);
    void logError(const char* format, ...);
}