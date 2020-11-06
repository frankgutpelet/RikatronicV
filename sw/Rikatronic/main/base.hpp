#include <ESP8266WebServer.h>
#include <Arduino.h>

class base
{
	public:
		base (ESP8266WebServer* server);

		void Render (void);
		void SetCallback_submit (void (*callback)(void));
		void Set_state (String value);
		String Get_state ();
		void Set_flap (String value);
		String Get_flap ();
		void Set_duration (String value);
		String Get_duration ();
		void Set_calibrated (String value);
		String Get_calibrated ();
		void Set_program (String value);
		String Get_program ();
		void Set_temp (String value);
		String Get_temp ();
	private:
		void Submit_Callback(void);
		int  ReplaceJSVariable(int index, const char * varName, const char * value, int valLength);
		void(*submit_UserCallback)(void);
		void Replace(String var, String val);
		ESP8266WebServer* server;

		String state;
		String flap;
		String duration;
		String calibrated;
		String program;
		String temp;
};