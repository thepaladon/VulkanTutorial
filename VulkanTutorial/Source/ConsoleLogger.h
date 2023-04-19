#pragma once
#include <cstdarg>

// This entire logger is written with ChatGPT 3.5 :p
// ...aaaaand a bit of manual debugging 

namespace Logger {

    enum class MessageType {
        INFO,
        DEBUG,
        WARNING,
        ERROR
    };

    //How do I make those PRIVATE and not warn users with dumb comments?!??!

    //DEPRECIATED FUNCTION, USE MACRO "LOG_DEBUG" instead
    void logInfo(const char* file, int line, const char* format, ...);

    //DEPRECIATED FUNCTION, USE MACRO "LOG_WARNING" instead
	void logDebug(const char* file, int line, const char* format, ...);

    //DEPRECIATED FUNCTION, USE MACRO "LOG_WARNING" instead
	void logWarning(const char* file, int line, const char* format, ...);

    //DEPRECIATED FUNCTION, USE MACRO "LOG_ERROR" instead
	void logError(const char* file, int line, const char* format, ...);

}


#define LOG_INFO(format, ...) \
    Logger::logInfo(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) \
    Logger::logDebug(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) \
    Logger::logWarning(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) \
    Logger::logError(__FILE__, __LINE__, format, ##__VA_ARGS__)

