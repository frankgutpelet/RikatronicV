#include <ESP8266WebServer.h>

class base
{
	public:
		base (ESP8266WebServer* server);

		void Render (void);		Settemp (String value);
		Setprogram (String value);
		Settime (String value);
		Setflap (String value);
		Setmode (String value);
		Setflap (String value);
	private:
		ESP8266WebServer* server;		String temp;
		String program;
		String time;
		String flap;
		String mode;
		String flap;
}