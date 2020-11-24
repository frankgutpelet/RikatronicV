#include "Arduino.h"
#ifndef LOGGER_H
# define LOGGER_H

#define LOGLEVEL_DEBUG

class Logger
{
public:
	// Returns a reference to the singleton Logger object
	static Logger* instance();

	void Debug(String logstr);

	void Error(String logstr);

  Logger();


protected:

private:
	void Log(String logstr);

};

#endif
