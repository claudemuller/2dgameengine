#include <iostream>
#include <iomanip>
#include "Logger.h"

const char* RESET = "\033[0m";
const char* RED = "\033[31m";

enum LogType {
    INFO,
    ERR,
} LogType;

const std::string emojis[] = {"ℹ️ ", "☠️ "};
const std::string typeStrs[] = {"INFO", "ERR"};

void printMessage(enum LogType type, std::string message, const char* colour) {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    std::cout << emojis[type] << colour << typeStrs[type] << " [" << std::put_time(&tm, "%d/%b/%Y %H:%M:%S") << "] - " << message << RESET << std::endl;
}

void Logger::Log(const std::string &message) {
    printMessage(LogType::INFO, message, RESET);
}

void Logger::Err(const std::string &message) {
    printMessage(LogType::ERR, message, RED);
}
