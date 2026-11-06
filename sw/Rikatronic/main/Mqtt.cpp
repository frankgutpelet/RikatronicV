#include "Mqtt.hpp"

static const unsigned long RETRY_MS = 10000;
// Retained Befehle (liegen noch auf dem Broker) kommen direkt nach dem Abonnieren an.
// Sie duerfen nicht ausgefuehrt werden, sonst wiederholt sich ein alter Befehl bei jedem Verbinden.
static const unsigned long RETAINED_WINDOW_MS = 2000;

MqttBridge::MqttBridge() : client(wifi)
{
	this->cfg = NULL;
	this->commandCallback = NULL;
	this->lastAttempt = 0;
	this->subscribedAt = 0;
	this->afterConnect = false;
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
	this->values[this->count][0] = 0;
	this->dirty[this->count] = false;
	return this->count++;
}

void MqttBridge::Publish(int id, const char* value)
{
	if (id < 0 || id >= this->count || NULL == value || 0 == value[0])
	{
		return;
	}
	// beim ersten Mal (leerer Zwischenspeicher) immer senden
	if (0 != strcmp(this->values[id], value) || 0 == this->values[id][0])
	{
		strlcpy(this->values[id], value, sizeof(this->values[id]));
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
	StackString<64> base(this->cfg->topic);
	StackString<72> lwt(base.c_str());
	lwt.append("/status");
	StackString<80> subTopic(base.c_str());
	subTopic.append("/+/set");
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
		Serial.print("MQTT connect failed, rc=");
		Serial.println(this->client.state());
	}
	if (ok)
	{
		this->client.publish(lwt.c_str(), "online", true);
		this->client.subscribe(subTopic.c_str());
		this->subscribedAt = millis();
		this->afterConnect = true;
		// nach (Neu-)Verbindung alle Werte einmal senden
		for (int i = 0; i < this->count; i++)
		{
			this->dirty[i] = (0 != this->values[i][0]);
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
	StackString<80> t(topic);
	StackString<64> prefix(this->cfg->topic);
	prefix.append('/');
	if (!t.startsWith(prefix.c_str()) || !t.endsWith("/set"))
	{
		return;
	}
	char name[24];
	size_t nameLen = t.length() - 4 - prefix.length();
	if (t.length() < prefix.length() + 4 || nameLen == 0 || nameLen >= sizeof(name))
	{
		return;
	}
	memcpy(name, t.c_str() + prefix.length(), nameLen);
	name[nameLen] = 0;
	if (NULL != strchr(name, '/'))
	{
		return;
	}
	if (this->afterConnect && (millis() - this->subscribedAt) < RETAINED_WINDOW_MS)
	{
		Serial.print("MQTT ignored (retained command after connect): ");
		Serial.println(name);
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
	for (int i = 0; i < this->count; i++)
	{
		if (this->dirty[i])
		{
			StackString<80> topic(this->cfg->topic);
			topic.append('/');
			topic.append(this->names[i]);
			if (this->client.publish(topic.c_str(), this->values[i], true))
			{
				this->dirty[i] = false;
			}
			else
			{
				Serial.print("MQTT publish failed: ");
				Serial.println(topic.c_str());
			}
			break;
		}
	}
}
