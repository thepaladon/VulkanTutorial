#include "ConsoleLogger.h"

#include <cassert>
#include <ctime>
#include <cstdarg>
#include <iomanip>
#include <sstream>

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

    void handleArgs(const char* format, va_list args)
    {
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

                case 'i':
                    std::printf("%i", va_arg(args, int));
                    break;

				default:
					std::printf("Percent '%c' Not implemented", *p);
                    assert(false);
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

    void handleType(MessageType type)
    {
        const char* typeStr = messageData[static_cast<int>(type)].typeStr;
        const int colorCode = messageData[static_cast<int>(type)].colorCode;

        printf("[\033[38;2;%d;%d;%dm%s\033[0m]", (colorCode >> 16) & 0xFF, (colorCode >> 8) & 0xFF, colorCode & 0xFF, typeStr);
    }

    std::string handleTime()
    {
        std::time_t t = std::time(nullptr);
        struct tm timeInfo;
        localtime_s(&timeInfo, &t);
        char timeStr[100];
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeInfo);
        return static_cast<std::string>(timeStr);
    }

	void logMessage(MessageType type, const char* file, int line, const char* format, va_list args) {
        handleType(type);
        const std::string timeStr = handleTime();
	    std::printf(" [%s] [%s:%d] ", timeStr.c_str(), extractFileName(file).c_str(), line);
    	handleArgs(format, args);
	}

    void logMessageNoFile(MessageType type, const char* format, va_list args) {
        handleType(type);
        const std::string timeStr = handleTime();
        std::printf(" [%s] ", timeStr.c_str());
        handleArgs(format, args);
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

     void VklogInfo(const char* file, int line, const char* format, ...) {
         va_list args;
         va_start(args, format);
         logMessageNoFile(MessageType::VK_INFO,  format, args);
         va_end(args);
     }

     void VklogDebug(const char* file, int line, const char* format, ...) {
         va_list args;
         va_start(args, format);
         logMessageNoFile(MessageType::VK_DEBUG,  format, args);
         va_end(args);
     }

     void VklogWarning(const char* file, int line, const char* format, ...) {
         va_list args;
         va_start(args, format);
         logMessageNoFile(MessageType::VK_WARNING,  format, args);
         va_end(args);
     }

     void VklogError(const char* file, int line, const char* format, ...)
     {
         va_list args;
         va_start(args, format);
         logMessageNoFile(MessageType::VK_ERROR,  format, args);
         va_end(args);
     }

}
