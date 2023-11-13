#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "base.hpp"
#include "flameRegulator.hpp"
#include "button.hpp"

const char* ssid = "WMOSKITO";
const char* password = ".ubX54bVSt#vxW11m.";
const char* myhostname = "RikatronicV";
const char* curVersion = "3.0.1"; //updateValues
const char* PushMessage_DeviceId = "vCDFC7D7572A7731";

ESP8266WebServer server(80);
base indexPage(&server);
FlameRegulator flameRegulator(&indexPage);
Button RefillButton;

void handleRoot() 
{
  if(String("MANUAL") == indexPage.Get_state())
  {
    flameRegulator.SetFlapRegulationMode(FlameRegulator::FLAP_MODE_MANUAL);
    String flapstate = indexPage.Get_flap();
    if(String("") != flapstate)
    {
        flameRegulator.SetManual(flapstate.toInt());
    }
  }
  else if (String("ECO")== indexPage.Get_state())
  {
      flameRegulator.SetFlapRegulationMode(FlameRegulator::FLAP_MODE_ECO);
  }
  else if (String("POWER")== indexPage.Get_state())
  {
      flameRegulator.SetFlapRegulationMode(FlameRegulator::FLAP_MODE_POWER);
  }
  if (String("REFILL")== indexPage.Get_program())
  {
    Serial.println("refill");
    flameRegulator.Refill();
  }
  else if (String("TEST") == indexPage.Get_beep())
  {
      flameRegulator.Beep(true);
      delay(1000);
      flameRegulator.Beep(false);
  }
  else if (String("ON") == indexPage.Get_beep())
  {
      flameRegulator.mute = false;
  }
  else if (String("OFF") == indexPage.Get_beep())
  {
      flameRegulator.mute = true;
  }
  indexPage.Set_state(flameRegulator.GetFlapRegulationModeStr());
  indexPage.Set_program(flameRegulator.GetProgramStateStr());
  indexPage.Set_flap(String(flameRegulator.GetFlapPosition()));
  indexPage.Set_temp(String(flameRegulator.GetTemperature()));
  indexPage.Set_beep(flameRegulator.mute ? "OFF" : "ON");
  indexPage.Set_version(curVersion);
  indexPage.Render();
}

void callback() 
{ 
  handleRoot();
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

void setup(void) 
{

  /* set all values for webpage */
  indexPage.Set_program("ECO");
  indexPage.Set_state("Off");
  indexPage.Set_flap("0");
  indexPage.Set_temp("20");
  indexPage.Set_duration("00:00");
  
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

  server.on("/", handleRoot);
    server.send(200, "text/plain", "this works as well");
  indexPage.SetCallback_submit(callback); 
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
  if (RefillButton.IsPressed())
  {
    Serial.println("refill");
    flameRegulator.Refill();
    delay(200);
  }
}
