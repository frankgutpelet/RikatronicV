#include "flap.hpp"
#include <Arduino.h>
// Benni Ofen
//#define PWM_FACTOR (((double)1023 * 4) / (100 * 21))
//#define PWM_OFFSET 49
// Frank Ofen
#define PWM_FACTOR (((double)1023) / (100 * 24 ))
#define PWM_OFFSET 0
// HW version 1
//#define PWM_FACTOR (((double)1023) / (100 * 24))
//#define PWM_OFFSET 0

Flap::Flap(int pwmPin, int relaisPin)
{
  this->pwmPin = pwmPin;
  this->relaisPin = relaisPin;
  this->logger = Logger::instance();
  this->position = 0;
  this->relayOn = false;

  pinMode(relaisPin, OUTPUT);
  this->SwitchOff();
}

void Flap::SetPosition(int percentage)
{
  if (percentage != this->position)
  {
    StackString<48> m("[flap] position "); m.append(this->position); m.append(" -> "); m.append(percentage);
    m.append(" @"); m.append((long)(millis() / 1000)); m.append("s");
    this->logger->Debug(m.c_str());
  }
  this->position = percentage;
  analogWrite(this->pwmPin, ((percentage * PWM_FACTOR)+PWM_OFFSET));
}

void Flap::SwitchOn()
{
  if (!this->relayOn)
  {
    StackString<48> m("[flap] RELAY ON @"); m.append((long)(millis() / 1000)); m.append("s");
    this->logger->Debug(m.c_str());
  }
  this->relayOn = true;
  digitalWrite(this->relaisPin, HIGH);
}

void Flap::SwitchOff()
{
  if (this->relayOn)
  {
    StackString<64> m("[flap] RELAY OFF @"); m.append((long)(millis() / 1000)); m.append("s pos=");
    m.append(this->position);
    this->logger->Debug(m.c_str());
  }
  this->relayOn = false;
  digitalWrite(this->relaisPin, LOW);
}

int Flap::GetPosition()
{
  return this->position;
}
