#include "flap.hpp"
#include <Arduino.h>
// Benni Ofen
#define PWM_FACTOR (((double)1023 * 4) / (100 * 21))
#define PWM_OFFSET 49
// Frank Ofen
//#define PWM_FACTOR (((double)1023 * 3) / (100 * 27))
//#define PWM_OFFSET 39
// HW version 1
//#define PWM_FACTOR (((double)1023) / (100 * 24))
//#define PWM_OFFSET 0

Flap::Flap(int pwmPin, int relaisPin)
{
  this->pwmPin = pwmPin;
  this->relaisPin = relaisPin;
  this->logger = Logger::instance();

  pinMode(relaisPin, OUTPUT);
  this->SwitchOff();
}

void Flap::SetPosition(int percentage)
{
  this->position = percentage;
  analogWrite(this->pwmPin, ((percentage * PWM_FACTOR)+PWM_OFFSET));
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
