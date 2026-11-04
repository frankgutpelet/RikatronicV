#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Save.hpp"

#define MQTT_MAX_VALUES 12

// Topics (<topic> = Basis-Topic aus der Weboberflaeche):
//   <topic>/<name>        publish (retained), nur bei Aenderung
//   <topic>/<name>/set    subscribe, Befehl an den Regler
//   <topic>/status        publish (retained), online / offline (Last Will)
class MqttBridge
{
	public:
		MqttBridge();

		void Begin(MqttConfig* config, String clientId);
		void SetCommandCallback(void (*callback)(const String& name, const String& value));
		int  Add(const char* name);                    // Wert anmelden, liefert ID
		void Publish(int id, const String& value);     // sendet nur bei Aenderung
		void Apply();                                  // nach geaenderter Konfiguration aufrufen
		void Loop();                                   // regelmaessig aus loop()
		const char* State();                           // "aus", "getrennt", "verbunden"

	private:
		void Connect();
		void OnMessage(char* topic, byte* payload, unsigned int length);

		WiFiClient wifi;
		PubSubClient client;
		MqttConfig* cfg;
		String clientId;
		void (*commandCallback)(const String& name, const String& value);

		unsigned long lastAttempt;
		int count;
		const char* names[MQTT_MAX_VALUES];
		String values[MQTT_MAX_VALUES];
		bool dirty[MQTT_MAX_VALUES];
};
