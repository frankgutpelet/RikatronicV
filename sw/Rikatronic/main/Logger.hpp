#include "Arduino.h"
#include "StackString.hpp"
#ifndef LOGGER_H
# define LOGGER_H

#define LOGLEVEL_DEBUG

class Logger
{
public:
	// Returns a reference to the singleton Logger object
	static Logger* instance();

	void Debug(const char* logstr);

	void Error(const char* logstr);

  Logger();


protected:

private:
	void Log(const char* logstr);

};

#endif
