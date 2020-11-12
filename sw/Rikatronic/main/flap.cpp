#include "flap.hpp"
#include <Arduino.h>
#define PWM_FACTOR (((double)1023 * 10) / (100 * 24))
#define PWM_OFFSET 90
#define TIMER_INTERVAL_MS        1000

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
