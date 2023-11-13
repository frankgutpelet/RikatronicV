#include "base.hpp"
#include <ArduinoJson.h>

StaticJsonDocument < 600 > base_doc;
const char base_text[] = "﻿<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n\
<html xmlns=\"http://www.w3.org/1999/xhtml\">\n\
\n\
<head>\n\
<meta content=\"de\" http-equiv=\"Content-Language\" />\n\
<meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\" />\n\
<title>Rikatronic V</title>\n\
<style type=\"text/css\">\n\
\n\
\n\
html {\n\
  background: linear-gradient(to bottom right, navy, black);\n\
 	background-repeat: no-repeat;\n\
 	min-height: 100%;\n\
}\n\
body {\n\
\n\
}\n\
\n\
.button {\n\
  display: inline-block;\n\
  padding: 15px 25px;\n\
  font-size: 24px;\n\
  cursor: pointer;\n\
  text-align: center;\n\
  text-decoration: none;\n\
  outline: none;\n\
  color: #fff;\n\
  background-color: silver;\n\
  border: none;\n\
  border-radius: 10px;\n\
  box-shadow: 0 9px darkblue;\n\
}\n\
.button2 {\n\
  display: inline-block;\n\
  padding: 30px 45px;\n\
  font-size: 35px;\n\
  cursor: pointer;\n\
  text-align: center;\n\
  text-decoration: none;\n\
  outline: none;\n\
  color: #fff;\n\
  background-color: silver;\n\
  border: none;\n\
  border-radius: 10px;\n\
  box-shadow: 0 9px darkblue;\n\
}\n\
.button:hover {\n\
	background-color: blueviolet\n\
}\n\
\n\
.button:active {\n\
  background-color: #3e8e41;\n\
  box-shadow: 0 5px #666;\n\
  transform: translateY(4px);\n\
}\n\
.auto-style-headline {\n\
    text-transform: uppercase;\n\
    font-family: verdana;\n\
    font-size: 3em;\n\
    font-weight: 700;\n\
    color: #f5f5f5;\n\
    text-shadow: 1px 1px 1px #919191,\n\
        1px 1px 1px #919191,\n\
        1px 2px 1px #919191,\n\
        1px 3px 1px #919191,\n\
        1px 4px 1px #919191,\n\
        1px 5px 1px #919191,\n\
\n\
    1px 18px 6px rgba(16,16,16,0.4),\n\
    1px 22px 10px rgba(16,16,16,0.2),\n\
    1px 25px 35px rgba(16,16,16,0.2);\n\
}\n\
\n\
.auto-style1 {\n\
	font-family: \"Gill Sans\", \"Gill Sans MT\", Calibri, \"Trebuchet MS\", sans-serif;\n\
	font-size: xx-large;\n\
	color: #FFFFFF;\n\
}\n\
.auto-style2 {\n\
    font-family: verdana;\n\
    font-size: 1em;\n\
    font-weight: 700;\n\
    color: #f5f5f5;\n\
    text-shadow: 1px 1px 1px #919191,\n\
    1px 5px 3px rgba(16,16,16,0.4),\n\
    1px 10px 5px rgba(16,16,16,0.2);\n\
}\n\
.auto-style-version {\n\
	text-align: right;\n\
	text-align: top;\n\
    font-family: verdana;\n\
    font-size: 0.6em;\n\
    font-weight: 700;\n\
    color: #f5f5f5;\n\
    text-shadow: 1px 1px 1px #919191,\n\
    1px 2px 1px rgba(16,16,16,0.4),\n\
    1px 4px 2px rgba(16,16,16,0.2);\n\
}\n\
.auto-style3 {\n\
	color: #FF0000;\n\
}\n\
.auto-style3 {\n\
	border-style: solid;\n\
	border-width: 1px;\n\
}\n\
.slider {\n\
  -webkit-appearance: none;\n\
  width: 100%;\n\
  height: 35px;\n\
  background: #silver;\n\
  outline: none;\n\
  opacity: 0.7;\n\
  -webkit-transition: .2s;\n\
  transition: opacity .2s;\n\
}\n\
.slider::-webkit-slider-thumb {\n\
  -webkit-appearance: none;\n\
  appearance: none;\n\
  width: 50px;\n\
  height: 35px;\n\
  background: darkblue;\n\
  cursor: pointer;\n\
}\n\
#CalibrationProgress {\n\
  width: 50%;\n\
  background-color: grey;\n\
  bottom: 20%;\n\
  position: fixed;\n\
  float: right;\n\
  visible: false;\n\
  box-shadow: 8px 8px 5px #444;\n\
}\n\
\n\
#CalibrationBar {\n\
  width: 1%;\n\
  height: 30px;\n\
  background-color: green;\n\
}\n\
\n\
</style>\n\
</head>\n\
<body>\n\
<p id=\"version\" class=\"auto-style-version\">Version: ?</p>\n\
<p class=\"auto-style-headline\" align=\"center\">Rikatronic V</p>\n\
<table class=\"auto-style3\" style=\"width: 100%\">\n\
	<tr>\n\
		<td class=\"auto-style2\" style=\"width: 484px\"><strong>Ofentemperatur</strong></td>\n\
		<td class=\"auto-style2\"><strong id=\"temp\"></strong><strong>°C</strong></td>\n\
	</tr>\n\
	<tr>\n\
		<td class=\"auto-style2\" style=\"width: 484px\"><strong>Programm</strong></td>\n\
		<td class=\"auto-style2\"><strong id=\"program\"></strong></td>\n\
	</tr>\n\
	<tr>\n\
		<td class=\"auto-style2\" style=\"width: 484px\"><strong>Brennzeit</strong></td>\n\
		<td class=\"auto-style2\"><strong id=\"duration\"></strong></td>\n\
	</tr>\n\
	<tr>\n\
		<td class=\"auto-style2\" style=\"width: 484px\"><strong>Klappe</strong></td>\n\
		<td id=\"flapColumn\" class=\"auto-style2\"><strong id=\"flap\"></strong><strong>%</strong></td>\n\
	</tr>\n\
	<tr>\n\
		<td class=\"auto-style2\" style=\"width: 484px\"><strong>Modus</strong></td>\n\
		<td class=\"auto-style2\"><strong id=\"state\"></strong></td>\n\
	</tr>\n\
	<tr>\n\
		<td class=\"auto-style2\" style=\"width: 484px\"><strong>Beep</strong></td>\n\
		<td class=\"auto-style2\"><strong id=\"beep\"></strong></td>\n\
	</tr>\n\
</table>\n\
</p>\n\
<table class=\"auto-style3\" style=\"width: 100%\">\n\
	<tr>\n\
		<td>\n\
			<form>\n\
				<p class=\"button\" type=\"submit\" onclick=\"power()\">power</p>\n\
			</form>\n\
		</td>\n\
		<td>\n\
			<form >\n\
				<p class=\"button\" type=\"submit\" onclick=\"manual()\">manuell</p>\n\
			</form>\n\
		</td>\n\
		<td>\n\
			<form>\n\
				<p class=\"button\" type=\"submit\" onclick=\"eco()\">eco +</p>\n\
			</form>\n\
		</td>\n\
		</tr>\n\
		<tr>\n\
		<td>\n\
			<form>\n\
				<p class=\"button\" type=\"submit\" onclick=\"refill()\">nachlegen</p>\n\
			</form>\n\
		</td>\n\
		<td>\n\
			<form>\n\
				<p class=\"button\" type=\"submit\" onclick=\"beep()\">beep</p>\n\
			</form>\n\
		</td>\n\
		<td>\n\
			<form>\n\
				<p class=\"button\" type=\"submit\" onclick=\"testBeep()\">test</p>\n\
			</form>\n\
		</td>\n\
	</tr>\n\
</table>\n\
</p>\n\
<table  class=\"auto-style2\" style=\"width: 100%\">\n\
	<tr>\n\
		<div class=\"slidecontainer\">\n\
			<input id=\"slider\" type=\"range\" min=\"0\" max=\"100\" value=0 class=\"slider\" onchange=\"sliderChange()\">\n\
		</div>\n\
	</tr>\n\
</table>\n\
</p>\n\
<table style=\"width: 100%\">\n\
	<tr>\n\
		<td>\n\
			<form action=\"submit\" method=\"post\" >\n\
				<button class=\"button2\" id=\"submitBtn\" type=\"submit\" name=\"action\" value=\"\" >übernehmen</button>\n\
			</form>\n\
		</td>\n\
		<td>\n\
			<form action=\"submit\" method=\"get\" >\n\
				<a href=\"http://rikatronicV\" class=\"button2\" >neu laden</a>\n\
			</form>\n\
		</td>\n\
	</tr>\n\
</table>\n\
\n\
\n\
</body>\n\
\n\
</html>\n\
<script>\n\
\n\
\n\
	var values = {\n\
		\"state\" : \"dynamic\",        \n\
		\"flap\" : \"dynamic\",         \n\
		\"slider\" : \"dynamic\",       \n\
		\"temp\" : \"dynamic\",         \n\
		\"program\" : \"dynamic\",      \n\
		\"duration\" : \"dynamic\",     \n\
		\"version\" : \"dynamic\",      \n\
		\"beep\" : \"dynamic\"}         \n\
\n\
	var editMode = false;\n\
\n\
	document.getElementById(\"version\").innerHTML = values.version\n\
	document.getElementById(\"flap\").innerHTML = values.flap\n\
	document.getElementById(\"slider\").value = values.flap\n\
	document.getElementById(\"temp\").innerHTML = values.temp\n\
	document.getElementById(\"state\").innerHTML = values.state\n\
	document.getElementById(\"duration\").innerHTML = values.duration\n\
	document.getElementById(\"program\").innerHTML = values.program\n\
	document.getElementById(\"beep\").innerHTML = values.beep\n\
	document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
\n\
	interval = setInterval(updateValues, 1000)\n\
\n\
	function refill()\n\
	{\n\
		editMode = true\n\
		values.program = \"REFILL\"\n\
		document.getElementById(\"state\").innerHTML = values.state\n\
		document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
	}\n\
	function power()\n\
	{\n\
		editMode = true\n\
		values.state = \"POWER\"\n\
		document.getElementById(\"state\").innerHTML = values.state\n\
		document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
	}\n\
	function eco()\n\
	{\n\
		editMode = true\n\
		values.state = \"ECO\"\n\
		document.getElementById(\"state\").innerHTML = values.state\n\
		document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
	}\n\
	function manual()\n\
	{\n\
		editMode = true\n\
		values.state = \"MANUAL\"\n\
		document.getElementById(\"state\").innerHTML = values.state\n\
		document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
	}\n\
	function beep()\n\
	{\n\
		editMode = true\n\
		if (\"ON\" == values.beep)\n\
		{\n\
			values.beep = \"OFF\"\n\
		}\n\
		else\n\
		{\n\
			values.beep = \"ON\"\n\
		}\n\
		document.getElementById(\"beep\").innerHTML = values.beep\n\
		document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
	}\n\
	function testBeep()\n\
	{\n\
		editMode = true\n\
		values.beep = \"TEST\"\n\
		document.getElementById(\"beep\").innerHTML = values.beep\n\
		document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
	}\n\
	function sliderChange()\n\
	{\n\
		if (\"MANUAL\" == values.state)\n\
		{\n\
			editMode = true\n\
			values.flap = document.getElementById(\"slider\").value\n\
			document.getElementById(\"flap\").innerHTML = values.flap\n\
			document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
		}\n\
		else\n\
		{\n\
			document.getElementById(\"slider\").value = values.flap\n\
		}\n\
	}\n\
\n\
	function updateValues()\n\
	{\n\
		var xhttp = new XMLHttpRequest();\n\
		xhttp.onreadystatechange = function(){\n\
			if (editMode)\n\
			{\n\
				return;\n\
			}\n\
			pairs = this.responseText.split(\";\");\n\
			for(let i = 0; i < pairs.length; i++)\n\
			{\n\
				pair = pairs[i].split(':');\n\
				values[pair[0]] = pair[1];\n\
				toChange = document.getElementById(pair[0])\n\
				if (null == toChange)\n\
				{\n\
					console.warn(\"Element \" + pair[0] + \"not found\");\n\
					continue\n\
				}\n\
				if (document.getElementById(pair[0]).type == \"range\")\n\
				{\n\
					document.getElementById(pair[0]).value = pair[1]\n\
				}\n\
				else\n\
				{\n\
\n\
					document.getElementById(pair[0]).innerHTML = pair[1]\n\
\n\
				}\n\
			}\n\
		}\n\
		xhttp.open(\"GET\", \"getValues\", true);\n\
		xhttp.send();\n\
	}\n\
\n\
</script>\n\
";

base::base(ESP8266WebServer* server)
{
	this->server = server;
	this->submit_UserCallback = (void(*)()) NULL;
	this->server->on("/submit", std::bind( & base::Submit_Callback, this));
	this->server->on("/getValues", std::bind( & base::GetAjaxValues, this));
}
void base::Submit_Callback(void)
{
	String jsonstring = this->server->arg("action");
	DeserializationError error = deserializeJson(base_doc, jsonstring);
	Serial.println(jsonstring);
	JsonObject obj = base_doc.as < JsonObject > ();

	if (error)
	{
	Serial.print(F("deserializeJson() failed: "));
	Serial.println(error.f_str());
	}
	else
	{
		this->state = obj["state"].as < String > ();
		this->program = obj["program"].as < String > ();
		this->beep = obj["beep"].as < String > ();
		this->slider = obj["slider"].as < String > ();
		this->duration = obj["duration"].as < String > ();
		this->temp = obj["temp"].as < String > ();
		this->version = obj["version"].as < String > ();
		this->flap = obj["flap"].as < String > ();

	}
	if (NULL != this->submit_UserCallback)
	{
		this->submit_UserCallback();
	}
}

void base::SetCallback_submit (void (*callback)(void))
{
	this->submit_UserCallback = callback;
}

void base::Set_state (String value)
{
	this->state = value;
}

String base::Get_state ( void )
{
	return this->state;
}
void base::Set_program (String value)
{
	this->program = value;
}

String base::Get_program ( void )
{
	return this->program;
}
void base::Set_beep (String value)
{
	this->beep = value;
}

String base::Get_beep ( void )
{
	return this->beep;
}
void base::Set_slider (String value)
{
	this->slider = value;
}

String base::Get_slider ( void )
{
	return this->slider;
}
void base::Set_duration (String value)
{
	this->duration = value;
}

String base::Get_duration ( void )
{
	return this->duration;
}
void base::Set_temp (String value)
{
	this->temp = value;
}

String base::Get_temp ( void )
{
	return this->temp;
}
void base::Set_version (String value)
{
	this->version = value;
}

String base::Get_version ( void )
{
	return this->version;
}
void base::Set_flap (String value)
{
	this->flap = value;
}

String base::Get_flap ( void )
{
	return this->flap;
}
void base::Render( void )
{
	this->server->send( 200, base_text);
}
void base::GetAjaxValues( void )
{
	String message = "state:" + this->state + ";program:" + this->program + ";beep:" + this->beep + ";slider:" + this->slider + ";duration:" + this->duration + ";temp:" + this->temp + ";version:" + this->version + ";flap:" + this->flap;
	this->server->send(200, "text/plain", message);
}
