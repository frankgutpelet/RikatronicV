#include "Logger.hpp"

class TempSensor
{
public:
	TempSensor(int analogInput);
	int GetTemp(void);
private:
	int analogInput;
	Logger* logger;
	double GetVoltage (void);
	double GetResistance(void);
};
