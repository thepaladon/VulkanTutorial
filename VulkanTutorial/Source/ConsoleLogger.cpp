#include "ConsoleLogger.h"

#include <iostream>
#include <ctime>
#include <cstdarg>

namespace Logger {
    std::string extractFileName(const std::string& path) {
        std::size_t pos = path.find_last_of("/\\");
        if (pos == std::string::npos) {
            return path;
        }
        else {
            return path.substr(pos + 1);
        }
    }

 void logMessage(MessageType type, const char* file, int line, const char* format, va_list args) {
        std::time_t t = std::time(nullptr);
        struct tm timeInfo;
        localtime_s(&timeInfo, &t);
        char timeStr[100];
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeInfo);

        const char* typeStr;
        const char* colorCode;
        switch (type) {
        case MessageType::INFO:
            typeStr = "INFO";
            colorCode = "\033[32m"; // green
            break;
        case MessageType::DEBUG:
            typeStr = "DEBUG";
            colorCode = "\033[34m"; // blue
            break;
        case MessageType::WARNING:
            typeStr = "WARNING";
            colorCode = "\033[33m"; // yellow
            break;
        case MessageType::ERROR:
            typeStr = "ERROR";
            colorCode = "\033[31m"; // red
            break;
        default:
            typeStr = "UNKNOWN";
            colorCode = "\033[0m"; // reset color
            break;
        }

        std::printf("[%s%s\033[0m] [%s] [%s:%d] ", colorCode, typeStr, timeStr, extractFileName(file).c_str(), line);
        for (const char* p = format; *p != '\0'; ++p) {
            if (*p == '%' && *(p + 1) != '\0') {
                ++p;
                switch (*p) {
                case 'd':
                    std::printf("%d", va_arg(args, int));
                    break;
                case 'f':
                    std::printf("%f", va_arg(args, double));
                    break;
                case 's':
                {
                    const char* str = va_arg(args, const char*);
                    std::printf("%s", str);
                    break;
                }
                case 'c':
                    std::printf("%c", va_arg(args, int));
                    break;
                default:
                    std::printf("INVALID");
                    break;
                }
            }
            else {
                std::printf("%c", *p);
            }
        }
        std::printf("\n");
        va_end(args);
    }


     void logInfo(const char* file, int line, const char* format, ...) {
        va_list args;
        va_start(args, format);
        logMessage(MessageType::INFO, file, line, format, args);
        va_end(args);
    }

     void logDebug(const char* file, int line, const char* format, ...) {
        va_list args;
        va_start(args, format);
        logMessage(MessageType::DEBUG, file, line, format, args);
        va_end(args);
    }

     void logWarning(const char* file, int line, const char* format, ...) {
        va_list args;
        va_start(args, format);
        logMessage(MessageType::WARNING, file, line, format, args);
        va_end(args);
	 }

     void logError(const char* file, int line, const char* format, ...)
     {
         va_list args;
         va_start(args, format);
         logMessage(MessageType::ERROR, file, line, format, args);
         va_end(args);
     }

}