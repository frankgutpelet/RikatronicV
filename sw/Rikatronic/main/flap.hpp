#include "Arduino.h"
#include "Logger.hpp"

extern "C" {
#include "user_interface.h"
}

class Flap
{
public:
	Flap(int pwmPin, int relaisPin);
	void SetPosition(int percentage);
	void SwitchOn();
	void SwitchOff();
	int GetPosition();

  
   
private:
  Logger* logger;
  int pwmPin, relaisPin;
	int position;
};
