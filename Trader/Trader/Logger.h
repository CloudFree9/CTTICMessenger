#ifndef __LOGGER__
#define __LOGGER__
class Logger {
public:
	Logger();
//	template<typename T> Logger& log(T msg); 
//	template<typename T> Logger& operator<<(T msg);
//	Logger& log(int msg);
//	Logger& log(long msg);
//	Logger& log(char *msg);
	friend Logger& operator <<(Logger& logger, char *msg);
	friend Logger& operator <<(Logger& logger, const char *msg);
	friend Logger& operator <<(Logger& logger, long msg);
	friend Logger& operator <<(Logger& logger, unsigned long msg);
	friend Logger& operator <<(Logger& logger, int msg);
	friend Logger& operator <<(Logger& logger, unsigned int msg);
};
#endif