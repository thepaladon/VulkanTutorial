#pragma once

#include <cassert>

namespace Logger {

    enum class MessageType {
        INFO,
        DEBUG,
        WARNING,
        ERROR,
        VK_INFO,
        VK_DEBUG,
        VK_WARNING,
        VK_ERROR
    };

    struct MessageData {
        const char* typeStr;
        int colorCode;
    };

    const MessageData messageData[] = {
    {"INFO", 0x00BFFF}, // added some cyan (blue-green) and made it brighter
    {"DEBUG", 0x00FF00}, // made it brighter
    {"WARNING", 0xFFD700}, // added some gold/yellow and made it brighter
    {"ERROR", 0xFF4040}, // added some pink/red and made it brighter
    {"VK_INFO", 0x00B3E6}, // added some cyan (blue-green) and made it slightly brighter than INFO
    {"VK_DEBUG", 0x00FF80}, // added some yellow-green and made it slightly brighter than DEBUG
    {"VK_WARNING", 0xFFC300}, // added some orange and made it slightly brighter and more saturated than WARNING
    {"VK_ERROR", 0xFF6666} // added some pink and made it slightly brighter and more saturated than ERROR
    };

    // USE MACRO "LOG_INFO" instead
    void logInfo(const char* file, int line, const char* format, ...);

    // USE MACRO "LOG_DEBUG" instead
	void logDebug(const char* file, int line, const char* format, ...);

    // USE MACRO "LOG_WARNING" instead
	void logWarning(const char* file, int line, const char* format, ...);

    // USE MACRO "LOG_ERROR" instead
	void logError(const char* file, int line, const char* format, ...);

    // USE MACRO "VK_LOG_INFO" instead
    void VklogInfo(const char* file, int line, const char* format, ...);
    
    // USE MACRO "VK_LOG_DEBUG" instead
    void VklogDebug(const char* file, int line, const char* format, ...);
    
    // USE MACRO "VK_LOG_WARNING" instead
    void VklogWarning(const char* file, int line, const char* format, ...);
    
    // USE MACRO "LOG_ERROR" instead
    void VklogError(const char* file, int line, const char* format, ...);

}


#define LOG_INFO(format, ...) \
    Logger::logInfo(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) \
    Logger::logDebug(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) \
    Logger::logWarning(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) \
    Logger::logError(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define VK_LOG_INFO(format, ...) \
    Logger::VklogInfo(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define VK_LOG_DEBUG(format, ...) \
    Logger::VklogDebug(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define VK_LOG_WARNING(format, ...) \
    Logger::VklogWarning(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define VK_LOG_ERROR(format, ...) \
    Logger::VklogError(__FILE__, __LINE__, format, ##__VA_ARGS__)

#define ASSERT(condition, format, ...) \
    do { \
        if (!(condition)) { \
			Logger::VklogError(__FILE__, __LINE__, format, ##__VA_ARGS__); \
			assert(false);\
        } \
    } while (false) \

