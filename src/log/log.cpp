#include "log/log.h"

namespace cpptools::log
{
    void Logger::setLevel(LogLevel level)
    {
        this->_level = level;
    }

    void Logger::setMode(LogMode mode)
    {
        this->_mode = mode;
    }

    void Logger::setFilename(const String &filename)
    {
        this->_filename = filename;
        for (const auto &level: {DEBUG, INFO, WARN, ERROR})
        {
            String newFilename = this->generateFileName(level);
            // 文件不存在则创建文件
            logFiles[level] = std::ofstream(newFilename, std::ios::app | std::ios::out);
            if (!logFiles[level].is_open())
            {
                throw std::runtime_error("Unable to open log file for " + levelString(level));
            }
            currentFilenames[level] = newFilename;
        }
    }

    void Logger::log(LogLevel level, const char *file, int line, const char *function, const char *format, ...)
    {
        if (level < this->_level)
        {
            return;
        }

        // 是否需要创建新文件
        this->checkNewFile(level);

        String levelStr = levelString(level);
        String timestamp = getCurrentTimestamp();

        va_list args;
        va_start(args, format);
        String message = formatString(format, args);
        va_end(args);

        String logMessage = "[" + timestamp + "] "
                            + "[" + levelStr + "] "
                            + "[" + file + ":" + std::to_string(line) + " (" + function + ")] "
                            + message;
        if (this->_mode == CONSOLE)
        {
            this->consoleOutput << logMessage << std::endl;
        } else if (this->_mode == FILE)
        {
            this->logFiles[level] << logMessage << std::endl;
        }
    }

    void Logger::debug(const char *file, int line, const char *function, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        getInstance().log(LogLevel::DEBUG, file, line, function, format, args);
        va_end(args);
    }

    void Logger::info(const char *file, int line, const char *function, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        getInstance().log(LogLevel::INFO, file, line, function, format, args);
        va_end(args);
    }

    void Logger::warn(const char *file, int line, const char *function, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        getInstance().log(LogLevel::WARN, file, line, function, format, args);
        va_end(args);
    }

    void Logger::error(const char *file, int line, const char *function, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        getInstance().log(LogLevel::ERROR, file, line, function, format, args);
        va_end(args);
    }

    Logger::~Logger()
    {
        if (this->_mode == FILE)
        {
            for (auto &logFile: logFiles)
            {
                if (logFile.second.is_open())
                {
                    logFile.second.close();
                }
            }
        }
    }

    String Logger::generateFileName(LogLevel level) const
    {
        std::time_t now = std::time(nullptr);
        char buf[128];
        // "%Y-%m-%d"
        std::strftime(buf, sizeof(buf), this->_filename.c_str(), std::localtime(&now));
        // {level} => 等级
        String result = std::string(buf) + ".log";
        cpptools::utils::replaceAll(result, "{level}", levelString(level));
        return result;
    }

    void Logger::checkNewFile(LogLevel level)
    {
        if (this->_mode != FILE)
        {
            return;
        }
        String newFilename = this->generateFileName(level);
        if (newFilename != this->currentFilenames[level])
        {
            if (logFiles[level].is_open())
            {
                logFiles[level].close();
            }
            this->currentFilenames[level] = newFilename;
            logFiles[level] = std::ofstream(newFilename);
            if (!logFiles[level].is_open())
            {
                throw std::runtime_error("Failed to open log file: " + newFilename);
            }
        }
    }

    void Logger::setSeparateFile(bool separateFile)
    {
        this->_separateFile = separateFile;
    }
}