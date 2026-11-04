#include <ESP8266WebServer.h>
#include <Arduino.h>
#pragma once
class base
{
	public:
		base (ESP8266WebServer* server);

		void Render (void);
		void GetAjaxValues (void);
		void SetCallback_submit (void (*callback)(void));
		void Set_state (String value);
		String Get_state ();
		void Set_flap (String value);
		String Get_flap ();
		void Set_temp (String value);
		String Get_temp ();
		void Set_program (String value);
		String Get_program ();
		void Set_beep (String value);
		String Get_beep ();
		void Set_version (String value);
		String Get_version ();
		void Set_headline (String value);
		String Get_headline ();
		void Set_mqtt (String value);
		String Get_mqtt ();
	private:
		void Submit_Callback(void);
		void(*submit_UserCallback)(void);
		ESP8266WebServer* server;

		String state;
		String flap;
		String temp;
		String program;
		String beep;
		String version;
		String headline;
		String mqtt;
};