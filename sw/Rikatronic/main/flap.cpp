#include "flap.hpp"
#include <Arduino.h>

#define PWM_FACTOR ((double)255 * 10) / (100 * 24))

Flap::Flap(int pwmPin, int relaisPin, int adcPin)
{
  this->pwmPin = pwmPin;
  this->relaisPin = relaisPin;
  this->adcPin = adcPin;
  
  pinMode(pwmPin, OUTPUT);
  pinMode(relaisPin, OUTPUT);
  pinMode(adcPin, INPUT);

  this->isOn = false;
  this->isCalibrated = false;
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

  return (int)((double)analogRead(adcPin) - this->calibrationFactorLow) * 100 / (this->calibrationFactorHigh - this->calibrationFactorLow);
}

void Flap::Calibrate()
{
  this->SwitchOff();
  delay(10000);
  this->calibrationFactorLow = analogRead(adcPin);
  this->SwitchOn(); 
  this->SetPosition(100);
  delay(120000);
  this->calibrationFactorHigh = analogRead(adcPin);
  this->isCalibrated = true;
}
