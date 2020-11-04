#include <ESP8266WebServer.h>
#include <Arduino.h>

class base
{
	public:
		base (ESP8266WebServer* server);

		void Render (void);
		void SetCallback_submit (void (*callback)(void));
		void Set_time (String value);
		void Set_mode (String value);
		void Set_program (String value);
		void Set_temp (String value);
		void Set_flap (String value);
	private:
    void Submit_Callback(void);
    int ReplaceJSVariable(int index, const char* varName, const char* value, int valLength);
    void (*Submit_UserCallback)(void);
    void Replace(String var, String val);
		ESP8266WebServer* server;		
		String duration;
		String state;
		String program;
		String temp;
		String flap;
};
