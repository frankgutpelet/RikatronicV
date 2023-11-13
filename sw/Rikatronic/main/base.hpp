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
		void Set_beep (String value);
		String Get_beep ();
		void Set_duration (String value);
		String Get_duration ();
		void Set_program (String value);
		String Get_program ();
		void Set_slider (String value);
		String Get_slider ();
		void Set_flap (String value);
		String Get_flap ();
		void Set_temp (String value);
		String Get_temp ();
		void Set_version (String value);
		String Get_version ();
	private:
		void Submit_Callback(void);
		void(*submit_UserCallback)(void);
		ESP8266WebServer* server;

		String state;
		String beep;
		String duration;
		String program;
		String slider;
		String flap;
		String temp;
		String version;
};