#include "Mqtt.hpp"

static const unsigned long RETRY_MS = 10000;

MqttBridge::MqttBridge() : client(wifi)
{
	this->cfg = NULL;
	this->commandCallback = NULL;
	this->lastAttempt = 0;
	this->count = 0;
}

void MqttBridge::Begin(MqttConfig* config, String clientId)
{
	this->cfg = config;
	this->clientId = clientId;
	// kurze Timeouts, damit ein nicht erreichbarer Broker die Regelung nicht blockiert
	this->wifi.setTimeout(2000);
	this->client.setSocketTimeout(2);
	this->client.setCallback([this](char* topic, byte* payload, unsigned int length)
	{
		this->OnMessage(topic, payload, length);
	});
	this->Apply();
}

void MqttBridge::SetCommandCallback(void (*callback)(const String& name, const String& value))
{
	this->commandCallback = callback;
}

int MqttBridge::Add(const char* name)
{
	if (this->count >= MQTT_MAX_VALUES)
	{
		return -1;
	}
	this->names[this->count] = name;
	this->dirty[this->count] = false;
	return this->count++;
}

void MqttBridge::Publish(int id, const String& value)
{
	if (id < 0 || id >= this->count || value.length() == 0)
	{
		return;
	}
	// beim ersten Mal (leerer Zwischenspeicher) immer senden
	if (this->values[id] != value || this->values[id].length() == 0)
	{
		this->values[id] = value;
		this->dirty[id] = true;
	}
}

void MqttBridge::Apply()
{
	if (this->client.connected())
	{
		this->client.disconnect();
	}
	this->client.setServer(this->cfg->host, this->cfg->port);
	this->lastAttempt = 0;
}

const char* MqttBridge::State()
{
	if (!this->cfg->enabled || 0 == this->cfg->host[0])
	{
		return "aus";
	}
	return this->client.connected() ? "verbunden" : "getrennt";
}

void MqttBridge::Connect()
{
	String base = String(this->cfg->topic);
	String lwt = base + "/status";
	bool ok;

	if (0 != this->cfg->user[0])
	{
		ok = this->client.connect(this->clientId.c_str(), this->cfg->user, this->cfg->pass, lwt.c_str(), 0, true, "offline");
	}
	else
	{
		ok = this->client.connect(this->clientId.c_str(), NULL, NULL, lwt.c_str(), 0, true, "offline");
	}

	if (!ok)
	{
		// rc: -2 Broker nicht erreichbar, 4 falsche Zugangsdaten, 5 nicht autorisiert
		Serial.println(String("MQTT connect failed, rc=") + this->client.state());
	}
	if (ok)
	{
		this->client.publish(lwt.c_str(), "online", true);
		this->client.subscribe((base + "/+/set").c_str());
		// nach (Neu-)Verbindung alle Werte einmal senden
		for (int i = 0; i < this->count; i++)
		{
			this->dirty[i] = (this->values[i].length() > 0);
		}
	}
}

void MqttBridge::OnMessage(char* topic, byte* payload, unsigned int length)
{
	if (NULL == this->commandCallback || length == 0 || length > 16)
	{
		return;
	}
	// Topic: <base>/<name>/set
	String t = String(topic);
	String prefix = String(this->cfg->topic) + "/";
	if (!t.startsWith(prefix) || !t.endsWith("/set"))
	{
		return;
	}
	String name = t.substring(prefix.length(), t.length() - 4);
	if (name.length() == 0 || name.indexOf('/') >= 0)
	{
		return;
	}
	char buf[17];
	memcpy(buf, payload, length);
	buf[length] = 0;
	String value = String(buf);
	value.trim();
	this->commandCallback(name, value);
}

void MqttBridge::Loop()
{
	if (!this->cfg->enabled || 0 == this->cfg->host[0] || WiFi.status() != WL_CONNECTED)
	{
		if (this->client.connected())
		{
			this->client.disconnect();
		}
		return;
	}

	if (!this->client.connected())
	{
		unsigned long now = millis();
		if (0 != this->lastAttempt && (now - this->lastAttempt) < RETRY_MS)
		{
			return;
		}
		this->lastAttempt = now;
		this->Connect();
		if (!this->client.connected())
		{
			return;
		}
	}

	this->client.loop();

	// pro Durchlauf nur einen Wert senden, damit die Verbindung zwischendurch
	// arbeiten kann; bei Fehlern bleibt der Wert markiert und wird wiederholt
	String base = String(this->cfg->topic);
	for (int i = 0; i < this->count; i++)
	{
		if (this->dirty[i])
		{
			String topic = base + "/" + this->names[i];
			if (this->client.publish(topic.c_str(), this->values[i].c_str(), true))
			{
				this->dirty[i] = false;
			}
			else
			{
				Serial.println(String("MQTT publish failed: ") + topic);
			}
			break;
		}
	}
}
