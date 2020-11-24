#include "Logger.hpp"
#include "Arduino.h"

// Static variable for the one-and-only instance
static Logger* pInstance;

Logger::Logger()
{
   Serial.begin(115200);
}
Logger* Logger::instance()
{
	if (pInstance == nullptr)
		pInstance = new Logger();
	return pInstance;
}

void Logger::Debug(String logstr)
{
#ifdef LOGLEVEL_DEBUG
	this->Log(logstr);
#else
	return;
#endif
}

void Logger::Error(String logstr)
{
#ifdef LOGLEVEL_DEBUG
	this->Log(logstr);
#elif defined LOGLEVEL_ERROR
	this->Log(logstr);
#else
	return;
#endif
}

void Logger::Log(String logstr)
{
	Serial.println(logstr);
}
