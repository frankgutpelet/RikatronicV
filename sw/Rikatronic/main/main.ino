#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "base.hpp"
#include "flap.hpp"

const char* ssid = "WMOSKITO";
const char* password = ".ubX54bVSt#vxW11m.";

ESP8266WebServer server(80);
Flap flap(5,3,0,1);

base indexPage(&server);

void handleRoot() 
{
   /* run calibration  */
  if ((Flap::FLAP_PROGRAM_STATE_CALIBRATION_LOW == flap.GetState())||(Flap::FLAP_PROGRAM_STATE_CALIBRATION_HIGH == flap.GetState()))
  {
    Serial.println("calibration Running");
    indexPage.Set_calibrated("progress"); 
  }
  else if (String("do") == indexPage.Get_calibrated())
  {
    Serial.println("do");
    indexPage.Set_calibrated("progress"); 
    flap.Calibrate();   
  }
  if(String("MANUAL") == indexPage.Get_state())
  {
    flap.SetMode(Flap::FLAP_MODE_MANUAL);
    String flapstate = indexPage.Get_flap();
    Serial.println(String("set manual flap to ") + flapstate + ("%"));
    if(String("") != flapstate)
    {
      flap.SetPosition(flapstate.toInt());
    }
  }

  if (flap.IsCalibrated())
  {
    Serial.println("calibrated");
    indexPage.Set_calibrated("true");
    indexPage.Set_flap(String(flap.GetPosition()));
  }
  indexPage.Set_temp(String(flap.GetTemp()));
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

void setup(void) {

  /* set all values for webpage */
  indexPage.Set_program("ECO");
  indexPage.Set_state("Off");
  indexPage.Set_flap("0");
  indexPage.Set_temp("20");
  indexPage.Set_duration("00:00");
  indexPage.Set_calibrated("false");
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.hostname("RikatronicV");
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
    server.send(200, "text/plain", "this works as well");
  indexPage.SetCallback_submit(callback); 

  server.on("/gif", []() {
    static const uint8_t gif[] PROGMEM = {
      0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x10, 0x00, 0x10, 0x00, 0x80, 0x01,
      0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x2c, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x00, 0x10, 0x00, 0x00, 0x02, 0x19, 0x8c, 0x8f, 0xa9, 0xcb, 0x9d,
      0x00, 0x5f, 0x74, 0xb4, 0x56, 0xb0, 0xb0, 0xd2, 0xf2, 0x35, 0x1e, 0x4c,
      0x0c, 0x24, 0x5a, 0xe6, 0x89, 0xa6, 0x4d, 0x01, 0x00, 0x3b
    };
    char gif_colored[sizeof(gif)];
    memcpy_P(gif_colored, gif, sizeof(gif));
    // Set the background to a random set of colors
    gif_colored[16] = millis() % 256;
    gif_colored[17] = millis() % 256;
    gif_colored[18] = millis() % 256;
    server.send(200, "image/gif", gif_colored, sizeof(gif_colored));
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
