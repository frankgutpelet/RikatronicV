#pragma once
#include <Arduino.h>
#include <ESP_EEPROM.h>

typedef struct
{
  bool enabled;
  char host[64];
  uint16_t port;
  char user[32];
  char pass[64];
  char topic[48];
} MqttConfig;

// Speichert nur die MQTT-Konfiguration (der Ofen hat sonst keine einstellbaren Werte)
class SaveEEprom
{
  typedef struct
  {
    uint32_t magic;
    MqttConfig mqtt;
  } dataStruct;

	public:
    SaveEEprom();
	  void Save();
    void Read();
    MqttConfig* Mqtt();

	private:
    void Defaults();
    MqttConfig mqtt;
};
