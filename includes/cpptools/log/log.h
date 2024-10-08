#pragma once

#include <fstream>
#include <iostream>
#include <cstdarg>
#include "cpptools/common/common.h"
#include "cpptools/utils/strings.h"

using namespace cpptools::common;

namespace cpptools::log
{
    enum LogLevel
    {
        DEBUG,
        INFO,
        WARN,
        ERROR
    };

    enum LogMode
    {
        // 控制台
        CONSOLE,
        // 文件
        FILE,
        // 外部，例如 logstash， 暂未实现
        EXTERNAL
    };

    inline String levelString(LogLevel level)
    {
        switch (level)
        {
            case LogLevel::DEBUG:
                return "DEBUG";
            case LogLevel::INFO:
                return "INFO";
            case LogLevel::WARN:
                return "WARN";
            case LogLevel::ERROR:
                return "ERROR";
            default:
                return "UNKNOWN";
        }
    }

    static String getCurrentTimestamp()
    {
        std::time_t now = std::time(nullptr);
        char buf[80];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return buf;
    }

    class Logger
    {
    private:
        LogLevel _level{LogLevel::INFO};
        LogMode _mode{LogMode::CONSOLE};
        String _filename{};
        HashMap<LogLevel, std::ofstream> logFiles{};
        std::ostream &consoleOutput{std::cout};
        HashMap<LogLevel, String> currentFilenames{};
        bool _separateFile{false};

    private:
        Logger() = default;

        String generateFileName(LogLevel level) const;

        void checkNewFile(LogLevel level);

    public:
        ~Logger();

        void setLevel(LogLevel level);

        void setMode(LogMode mode);

        void setFilename(const String &filename);

        void setSeparateFile(bool separateFile);

        void log(LogLevel level, const char *file, int line, const char *function, const char *format, ...);

        void log(LogLevel level, const char *file, int line, const char *function,const String& content);

        static void debug(const char *file, int line, const char *function, const char *format, ...);

        static void info(const char *file, int line, const char *function, const char *format, ...);

        static void warn(const char *file, int line, const char *function, const char *format, ...);

        static void error(const char *file, int line, const char *function, const char *format, ...);

        static Logger &getInstance()
        {
            static Logger instance;
            return instance;
        }
    };

#define LOG_LEVEL(level) Logger::getInstance().setLevel(level)

#define LOG_MODE(mode) Logger::getInstance().setMode(mode)

#define LOG_FILENAME(filename) Logger::getInstance().setFilename(filename)

#define LOG(level, format, ...) \
    Logger::getInstance().log(level, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) \
    Logger::debug(__FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) \
    Logger::info(__FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) \
    Logger::warn(__FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) \
    Logger::error(__FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
}
