#ifndef LOGGER_H
#define LOGGER_H

#include <string>

class Logger {
private:

public:
	static void Log(const std::string &message);
	static void Err(const std::string &message);
};

#endif // LOGGER_H

