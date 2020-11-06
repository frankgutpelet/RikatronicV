#include "flap.hpp"
#include <Arduino.h>
#define PWM_FACTOR ((double)255 * 10) / (100 * 24))
#define TIMER_INTERVAL_MS        1000
#define TMP_CURVE_OFFSET 1525
#define TMP_CURVE_PITCH 1500
#define TMP_CURVE_FLATNESS 200


void timerCallback(void *pArg);

Flap::Flap(int pwmPin, int relaisPin, int adcPin, int adcTmpPin)
{
  this->pwmPin = pwmPin;
  this->relaisPin = relaisPin;
  this->adcPin = adcPin;
  this->adcTmpPin = adcTmpPin;
  
  pinMode(pwmPin, OUTPUT);
  pinMode(relaisPin, OUTPUT);
  pinMode(adcPin, INPUT);
  pinMode(adcTmpPin, INPUT);
  this->isOn = false;
  this->isCalibrated = false;
  os_timer_setfn(&(this->timer), timerCallback, this);
  //os_timer_setfn(&(this->timer), std::bind( & Flap::SetPosition, this))
  os_timer_arm(&(this->timer), 1000, true);
  this->programState = FLAP_PROGRAM_STATE_NONE;
  this->action_timeSec = 0;
}

void Flap::SetPosition(int percentage)
{
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
  if(!this->isCalibrated)
  {
    return ERROR_NOT_CALIBRATED;
  }

  if (0 >= (this->calibrationFactorHigh - this->calibrationFactorLow))
  {
    return ERROR_WRONG_CALIBRATED;
  }
  return (int)((double)analogRead(adcPin) - this->calibrationFactorLow) * 100 / (this->calibrationFactorHigh - this->calibrationFactorLow);
}
bool Flap::IsCalibrated()
{
  return this->isCalibrated;
}

void Flap::Calibrate()
{
  this->SwitchOff();
  this->action_timeSec = 10;
  this->programState = FLAP_PROGRAM_STATE_CALIBRATION_LOW;
}

int Flap::GetTemp()
{
  return ((exp(this->GetTmpResistance()/TMP_CURVE_FLATNESS)*TMP_CURVE_PITCH)-TMP_CURVE_OFFSET);
}

double Flap::GetTmpResistance()
{
  double voltage = this->GetTmpVoltage();
  return (voltage * 100)/(3,3 - voltage);
}
double Flap::GetTmpVoltage()
{
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
  
  switch (this->programState)
  {
    case Flap::FLAP_PROGRAM_STATE_CALIBRATION_LOW: /* after 10sec  */
      this->calibrationFactorLow = analogRead(this->adcPin);
      this->SwitchOn(); 
      this->SetPosition(100);
      this->action_timeSec = 120;
      this->programState = Flap::FLAP_PROGRAM_STATE_CALIBRATION_HIGH;
      return;
    case Flap::FLAP_PROGRAM_STATE_CALIBRATION_HIGH: /* after 2 min  */
      this->calibrationFactorHigh = analogRead(this->adcPin);
      this->isCalibrated = true;
      this->programState = Flap::FLAP_PROGRAM_STATE_NONE;
      return;
    case Flap::FLAP_PROGRAM_STATE_NONE:
    default:
      return;
  }
}

void timerCallback(void *pArg)
{
  Flap* instance = (Flap*) pArg;
  instance->ProgramStateMachine();  
}
