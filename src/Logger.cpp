#include <iostream>
#include <iomanip>
#include "Logger.h"

const char* RESET = "\033[0m";
const char* RED = "\033[31m";
const char* YELLOW = "\033[33m";
const char* GREEN = "\033[32m";

const std::string emojis[] = {"ℹ️ ", "⚠️ ", "☠️ "};
const std::string typeStrs[] = {"INFO", "WARN", "ERR"};

std::vector<LogEntry> Logger::messages;

void printMessage(enum LogType type, std::string message, const char* colour) {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    LogEntry logEntry = {
	    .type = type,
	    .message = message
    };
    Logger::messages.push_back(logEntry);

    std::cout << emojis[type] << colour << typeStrs[type] << " [" << std::put_time(&tm, "%d/%b/%Y %H:%M:%S") << "] - " << message << RESET << std::endl;
}

void Logger::Info(const std::string &message) {
    printMessage(LogType::LOG_INFO, message, RESET);
}

void Logger::Warn(const std::string &message) {
    printMessage(LogType::LOG_WARN, message, YELLOW);
}

void Logger::Err(const std::string &message) {
    printMessage(LogType::LOG_ERR, message, RED);
}
