#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include "base.hpp"
#include "flameRegulator.hpp"
#include "button.hpp"
#include "Save.hpp"
#include "Mqtt.hpp"

const char* ssid = "WMOSKITO";
const char* password = ".ubX54bVSt#vxW11m.";
const char* myhostname = "RikatronicV";
const char* title = "Rikatronic V";
const char* curVersion = "4.0.0";
const char* PushMessage_DeviceId = "vCDFC7D7572A7731";

ESP8266WebServer server(80);
base indexPage(&server);
FlameRegulator flameRegulator;
Button RefillButton;
SaveEEprom storage;
MqttBridge mqtt;

// IDs der MQTT-Werte (<topic>/<name>)
int mqttTemp, mqttFlap, mqttMode, mqttProgram, mqttBeep;

#define BUTTON_LONG_PRESS_MS 3000   // so lange Taste halten = Anheizen

unsigned long lastPageUpdate = 0;
unsigned long buttonDownSince = 0; // 0 = Taste nicht gedrueckt
bool buttonLongDone = false;       // langer Druck wurde schon ausgewertet
unsigned long beepTestUntil = 0;   // 0 = kein Testton aktiv

// "on", "1", "true" -> true; "off", "0", "false" -> false
bool parseOnOff(String s, bool* out)
{
  s.trim();
  s.toLowerCase();
  if (s == "on" || s == "1" || s == "true")
  {
    *out = true;
    return true;
  }
  if (s == "off" || s == "0" || s == "false")
  {
    *out = false;
    return true;
  }
  return false;
}

// ganze Zahl 0-100
bool parsePercent(String s, int* out)
{
  s.trim();
  if (s.length() == 0 || s.length() > 3)
  {
    return false;
  }
  for (unsigned int i = 0; i < s.length(); i++)
  {
    if (!isDigit(s[i]))
    {
      return false;
    }
  }
  int v = s.toInt();
  if (v > 100)
  {
    return false;
  }
  *out = v;
  return true;
}

// Modus fuer MQTT/Homey: manual / eco / power
String modeName()
{
  String m = flameRegulator.GetFlapRegulationModeStr();
  m.toLowerCase();
  return m;
}

#define FEEDBACK_BEEP_MS 200   // Bestaetigungs-Piep

// Beeper nicht blockierend fuer ms Millisekunden einschalten (loop() schaltet wieder ab)
void startBeep(unsigned long ms)
{
  unsigned long until = millis() + ms;
  if (0 == until) until = 1;
  if (0 == beepTestUntil || (long)(until - beepTestUntil) > 0) beepTestUntil = until; // laengeren Piep nicht verkuerzen
  flameRegulator.beepHold = true;
  flameRegulator.Beep(true);
}

// kurzer Bestaetigungs-Piep, nur wenn der Signalton nicht stumm geschaltet ist
void feedbackBeep()
{
  if (flameRegulator.mute) return;
  startBeep(FEEDBACK_BEEP_MS);
}

// Befehl ausfuehren: gemeinsamer Weg fuer Weboberflaeche und MQTT (<name>/set)
// true = angenommen
bool applySetting(const String& name, const String& value)
{
  bool b;
  int v;

  if (name == "mode")
  {
    String m = value;
    m.trim();
    m.toUpperCase();
    if (m == "MANUAL") flameRegulator.SetFlapRegulationMode(FlameRegulator::FLAP_MODE_MANUAL);
    else if (m == "ECO") flameRegulator.SetFlapRegulationMode(FlameRegulator::FLAP_MODE_ECO);
    else if (m == "POWER") flameRegulator.SetFlapRegulationMode(FlameRegulator::FLAP_MODE_POWER);
    else return false;
  }
  else if (name == "flap")
  {
    // wie bisher nur im manuellen Modus
    if (FlameRegulator::FLAP_MODE_MANUAL != flameRegulator.GetFlapRegulationMode()) return false;
    if (!parsePercent(value, &v)) return false;
    flameRegulator.SetManual(v);
  }
  else if (name == "refill")
  {
    bool isOff = (parseOnOff(value, &b) && !b); // 0/off/false = nichts tun, alles andere loest aus
    if (!isOff)
    {
      Serial.println("refill");
      flameRegulator.Refill();
    }
  }
  else if (name == "heatup")
  {
    bool isOff = (parseOnOff(value, &b) && !b); // 0/off/false = nichts tun, alles andere loest aus
    if (!isOff)
    {
      // im manuellen Modus laeuft der Zustandsautomat nicht, Anheizen waere wirkungslos
      if (FlameRegulator::FLAP_MODE_MANUAL == flameRegulator.GetFlapRegulationMode()) return false;
      Serial.println("heat up");
      flameRegulator.Heat_Up();
      feedbackBeep();
    }
  }
  else if (name == "beep")
  {
    String s = value;
    s.trim();
    s.toUpperCase();
    if (s == "TEST")
    {
      // nicht blockierend: loop() schaltet nach 1 s wieder ab
      startBeep(1000);
    }
    else if (parseOnOff(s, &b))
    {
      flameRegulator.mute = !b;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
  return true;
}

void onMqttCommand(const String& name, const String& value)
{
  bool ok = applySetting(name, value);
  Serial.println(String("MQTT rx: ") + name + " = '" + value + "' -> " + (ok ? "ok" : "rejected"));
  feedbackBeep(); // jeder MQTT-Befehl wird mit kurzem Piep bestaetigt
}

// Weboberflaeche: POST /submit action={"state":"ECO"} / {"flap":"60"} / {"program":"REFILL"} / {"beep":"TEST"}
// nur die mitgeschickten Felder werden ausgefuehrt
void handleSubmit()
{
  StaticJsonDocument < 256 > doc;
  DeserializationError error = deserializeJson(doc, server.arg("action"));
  if (error)
  {
    server.send(400, "text/plain", "Bad request");
    return;
  }
  JsonObject obj = doc.as < JsonObject > ();
  bool ok = true;

  if (obj.containsKey("state"))
  {
    ok &= applySetting("mode", obj["state"].as < String > ());
  }
  if (obj.containsKey("flap"))
  {
    ok &= applySetting("flap", obj["flap"].as < String > ());
  }
  if (obj.containsKey("program") && String("REFILL") == obj["program"].as < String > ())
  {
    ok &= applySetting("refill", "ON");
  }
  if (obj.containsKey("beep"))
  {
    ok &= applySetting("beep", obj["beep"].as < String > ());
  }
  server.send(ok ? 200 : 400, "text/plain", ok ? "ok" : "Bad value");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void Render()
{
  indexPage.Render();
}

// GET /mqtt -> aktuelle MQTT-Konfiguration als JSON (ohne Passwort)
void sendMqttConfig()
{
  MqttConfig* c = storage.Mqtt();
  StaticJsonDocument < 512 > doc;
  doc["enabled"] = c->enabled;
  doc["host"] = c->host;
  doc["port"] = c->port;
  doc["user"] = c->user;
  doc["hasPass"] = (0 != c->pass[0]);
  doc["topic"] = c->topic;
  doc["state"] = mqtt.State();
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

// POST /mqtt -> Konfiguration speichern und MQTT neu verbinden
void saveMqttConfig()
{
  MqttConfig* c = storage.Mqtt();
  String host = server.arg("host");
  String user = server.arg("user");
  String pass = server.arg("pass");
  String topic = server.arg("topic");
  int port = server.arg("port").toInt();

  host.trim();
  topic.trim();
  if (host.length() >= sizeof(c->host) || user.length() >= sizeof(c->user) ||
      pass.length() >= sizeof(c->pass) || topic.length() >= sizeof(c->topic))
  {
    server.send(400, "text/plain", "Eingabe zu lang");
    return;
  }
  if (topic.length() == 0 || topic.startsWith("/") || topic.endsWith("/") ||
      topic.indexOf('#') >= 0 || topic.indexOf('+') >= 0)
  {
    server.send(400, "text/plain", "Ungueltiges Topic");
    return;
  }
  if (port < 1 || port > 65535)
  {
    port = 1883;
  }

  c->enabled = (server.arg("enabled") == "1");
  strlcpy(c->host, host.c_str(), sizeof(c->host));
  c->port = port;
  strlcpy(c->user, user.c_str(), sizeof(c->user));
  if (pass.length() > 0)
  {
    strlcpy(c->pass, pass.c_str(), sizeof(c->pass)); // leer = unveraendert
  }
  if (0 == c->user[0])
  {
    c->pass[0] = 0; // ohne Benutzer kein Passwort
  }
  strlcpy(c->topic, topic.c_str(), sizeof(c->topic));

  storage.Save();
  mqtt.Apply();
  sendMqttConfig();
}

// Werte fuer Weboberflaeche und MQTT aktualisieren (1x pro Sekunde)
void updatePage()
{
  int temp = flameRegulator.GetTemperature();
  int flap = flameRegulator.GetFlapPosition();
  String program = flameRegulator.GetProgramStateStr();

  indexPage.Set_state(flameRegulator.GetFlapRegulationModeStr());
  indexPage.Set_program(program);
  indexPage.Set_flap(String(flap));
  indexPage.Set_temp(String(temp));
  indexPage.Set_beep(flameRegulator.mute ? "OFF" : "ON");
  indexPage.Set_mqtt(mqtt.State());

  // MQTT: Werte werden nur bei Aenderung gesendet
  mqtt.Publish(mqttTemp, String(temp));
  mqtt.Publish(mqttFlap, String(flap));
  mqtt.Publish(mqttMode, modeName());
  mqtt.Publish(mqttProgram, program);
  mqtt.Publish(mqttBeep, flameRegulator.mute ? "OFF" : "ON");
}

// Taste: kurz druecken = Nachlegen (beim Loslassen), 3 s halten = Anheizen
void handleButton()
{
  if (RefillButton.IsPressed())
  {
    if (0 == buttonDownSince)
    {
      buttonDownSince = millis();
      if (0 == buttonDownSince) buttonDownSince = 1;
      buttonLongDone = false;
    }
    else if (!buttonLongDone && (millis() - buttonDownSince) >= BUTTON_LONG_PRESS_MS)
    {
      buttonLongDone = true;
      applySetting("heatup", "ON");
    }
  }
  else if (0 != buttonDownSince)
  {
    unsigned long held = millis() - buttonDownSince;
    buttonDownSince = 0;
    if (!buttonLongDone && held >= 30) // kurzer Druck, Prellen ignorieren
    {
      Serial.println("refill");
      flameRegulator.Refill();
    }
  }
}

void setup(void)
{
  /* set all values for webpage */
  indexPage.Set_headline(title);
  indexPage.Set_version(curVersion);
  indexPage.Set_state("ECO");
  indexPage.Set_program("");
  indexPage.Set_flap("0");
  indexPage.Set_temp("");
  indexPage.Set_beep("OFF");
  indexPage.Set_mqtt("aus");

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(myhostname);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266"))
  {
    Serial.println("MDNS responder started");
  }

  server.on("/", Render);
  server.on("/mqtt", HTTP_GET, sendMqttConfig);
  server.on("/mqtt", HTTP_POST, saveMqttConfig);
  indexPage.SetCallback_submit(handleSubmit);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  mqttTemp = mqtt.Add("temp");
  mqttFlap = mqtt.Add("flap");
  mqttMode = mqtt.Add("mode");
  mqttProgram = mqtt.Add("program");
  mqttBeep = mqtt.Add("beep");
  mqtt.SetCommandCallback(onMqttCommand);
  mqtt.Begin(storage.Mqtt(), String(myhostname) + "-" + String(ESP.getChipId(), HEX));

  updatePage();
}

void loop(void)
{
  server.handleClient();
  MDNS.update();
  mqtt.Loop();

  handleButton();

  if (0 != beepTestUntil && (long)(millis() - beepTestUntil) >= 0)
  {
    beepTestUntil = 0;
    flameRegulator.beepHold = false;
    flameRegulator.Beep(false);
  }

  if ((millis() - lastPageUpdate) >= 1000)
  {
    lastPageUpdate = millis();
    updatePage();
  }
}
