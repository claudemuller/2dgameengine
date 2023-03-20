#include <iostream>
#include <iomanip>
#include "Logger.h"

void Logger::Log(const std::string &message) {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::cout << "ℹ️ LOG: [" << std::put_time(&tm, "%d/%b/%Y %H:%M:%S") << "] - " << message << std::endl;
}

void Logger::Err(const std::string &message) {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::cout << "☠️ \033[1;31mERR: [" << std::put_time(&tm, "%d/%b/%Y %H:%M:%S") << "] - " << message << "\033[0m" << std::endl;
}
