#include <Arduino.h>
#include "tempSensor.hpp"

#define PT1000_0K_OFFSET (1000)
#define PT1000_CURVE_PITCH ((double) 0.28)
#define RESISTANCE_DEVIDER_R1 (1000)
#define SYSTEM_VOLTAGE ((double) 3.3)
#define ADC_RESOLUTION_HIGHEST 1023
#define TMP_OFFSET (-30)
#define TMP_FACTOR (1)

TempSensor::TempSensor(int analogInput)
{
	this->analogInput = analogInput;
	pinMode(analogInput, INPUT);
	logger = Logger::instance();
}
int TempSensor::GetTemp(void)
{
	double resistance = this->GetResistance();
	double temp = ((resistance - PT1000_0K_OFFSET) * PT1000_CURVE_PITCH);
  temp = ((temp * TMP_FACTOR) + TMP_OFFSET);
	logger->Debug(String("temperature: ") + String(temp));
	return (int) temp;
}

double TempSensor::GetResistance(void)
{
	double voltage = this->GetVoltage();
	double resistance = voltage * RESISTANCE_DEVIDER_R1 / (SYSTEM_VOLTAGE - voltage);
	//logger->Debug(String("Voltage: ") + String(voltage));
	//logger->Debug(String("Resistance: ") + String(resistance));
	return resistance;
}
double TempSensor::GetVoltage(void)
{
	double tempValue = analogRead(this->analogInput);
	//logger->Debug(String("Temp value: ") + String(tempValue));
	return (SYSTEM_VOLTAGE * tempValue / ADC_RESOLUTION_HIGHEST);
}

/* Kennlinie
ohm	961		1000	1039	1078	1117	1155	1194	1232	1271	1309	1347	1385	1423	1461	1499	1536	1573	1611
°C - 10		0		  10		20		30		40		50		60		70		80		90		100		110		120		130		140		150		160

*/
