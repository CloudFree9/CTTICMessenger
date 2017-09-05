#include <iostream>
#include "Logger.h"
using namespace std;

Logger::Logger() {
}

Logger& operator <<(Logger &logger, char *msg) {
	cout << msg;
	return logger;
}

Logger& operator <<(Logger &logger, const char *msg) {
	cout << msg;
	return logger;
}

Logger& operator <<(Logger &logger, long msg) {
	cout << msg;
	return logger;
}

Logger& operator <<(Logger &logger, unsigned long msg) {
	cout << msg;
	return logger;
}
Logger& operator <<(Logger &logger, int msg) {
	return logger << (long)msg;
}

Logger& operator <<(Logger &logger, unsigned int msg) {
	return logger << (unsigned long)msg;
}