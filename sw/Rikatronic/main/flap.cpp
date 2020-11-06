#include "flap.hpp"
#include <Arduino.h>
#define PWM_FACTOR ((double)1023 * 10) / (100 * 24))
#define TIMER_INTERVAL_MS        1000
#define TMP_CURVE_OFFSET 1525
#define TMP_CURVE_PITCH 1500
#define TMP_CURVE_FLATNESS 200


void timerCallback(void *pArg);

Flap::Flap(int pwmPin, int relaisPin, int adcTmpPin)
{
  this->pwmPin = pwmPin;
  this->relaisPin = relaisPin;
  this->adcTmpPin = adcTmpPin;
 
  pinMode(relaisPin, OUTPUT);;
  pinMode(adcTmpPin, INPUT);
  this->isOn = false;
  os_timer_setfn(&(this->timer), timerCallback, this);
  //os_timer_setfn(&(this->timer), std::bind( & Flap::SetPosition, this))
  os_timer_arm(&(this->timer), 1000, true);
  this->programState = FLAP_PROGRAM_STATE_NONE;
  this->action_timeSec = 0;
}

void Flap::SetPosition(int percentage)
{
  this->position = percentage;
  analogWrite(this->pwmPin, percentage * PWM_FACTOR;
}

void Flap::SwitchOn()
{
  digitalWrite(this->relaisPin, HIGH);
}

void Flap::SwitchOff()
{
  digitalWrite(this->relaisPin, LOW);
}

int Flap::GetPosition()
{
  return this->position;
}

int Flap::GetTemp()
{
  return ((exp(this->GetTmpResistance()/TMP_CURVE_FLATNESS)*TMP_CURVE_PITCH)-TMP_CURVE_OFFSET);
}

double Flap::GetTmpResistance()
{
  double voltage = this->GetTmpVoltage();
  Serial.println(String("Voltage: ") + String(voltage));
  Serial.println(String("Resistancee: ") + String((voltage * 100)/(3,3 - voltage)));
  return (voltage * 100)/(3,3 - voltage);
}
double Flap::GetTmpVoltage()
{
  Serial.println(String("Temp value: ") + String(analogRead(this->adcTmpPin)));
  return 3,3 * analogRead(this->adcTmpPin) / 1023;
}
void Flap::SetMode(flapMode_e state)
{
  this->state = state;
}

void Flap::ProgramStateMachine()
{
  if (0 != this->action_timeSec)
  {
    this->action_timeSec--;
    return;
  }
}

void timerCallback(void *pArg)
{
  Flap* instance = (Flap*) pArg;
  instance->ProgramStateMachine();  
}
