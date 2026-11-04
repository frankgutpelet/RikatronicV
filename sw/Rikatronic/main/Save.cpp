#include <cstring>
#include "Save.hpp"
#include <ESP8266WiFi.h>

static const uint32_t SAVE_MAGIC = 0x4F46454E; // aendern, wenn sich dataStruct aendert

SaveEEprom::SaveEEprom()
{
  this->Read();
}

void SaveEEprom::Defaults()
{
  memset(&this->mqtt, 0, sizeof(this->mqtt));
  this->mqtt.port = 1883;
  strncpy(this->mqtt.topic, "ofen", sizeof(this->mqtt.topic) - 1);
}

void SaveEEprom::Read()
{
  dataStruct storage;

  EEPROM.begin(sizeof(storage));
  EEPROM.get(0, storage);
  EEPROM.end();

  if (storage.magic != SAVE_MAGIC)
  {
    // leerer Speicher oder fremdes Layout
    this->Defaults();
    return;
  }
  this->mqtt = storage.mqtt;
  // Strings sicher terminieren
  this->mqtt.host[sizeof(this->mqtt.host) - 1] = 0;
  this->mqtt.user[sizeof(this->mqtt.user) - 1] = 0;
  this->mqtt.pass[sizeof(this->mqtt.pass) - 1] = 0;
  this->mqtt.topic[sizeof(this->mqtt.topic) - 1] = 0;
  if (this->mqtt.port == 0)
  {
    this->mqtt.port = 1883;
  }
}

void SaveEEprom::Save()
{
  dataStruct storage;

  storage.magic = SAVE_MAGIC;
  storage.mqtt = this->mqtt;

  EEPROM.begin(sizeof(storage));
  EEPROM.put(0, storage);
  EEPROM.commit();
  EEPROM.end();
}

MqttConfig* SaveEEprom::Mqtt()
{
  return &this->mqtt;
}
