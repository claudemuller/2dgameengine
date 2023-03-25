#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>

enum LogType {
    LOG_INFO,
    LOG_WARN,
    LOG_ERR,
};

struct LogEntry {
    LogType type;
    std::string message;
};

class Logger {
private:

public:
    static bool Log;
    static std::vector<LogEntry> messages;
    static void Info(const std::string &message);
    static void Warn(const std::string &message);
    static void Err(const std::string &message);
};

#endif // LOGGER_H

