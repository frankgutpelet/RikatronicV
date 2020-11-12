#include "base.hpp"
#include <ArduinoJson.h>

StaticJsonDocument < 300 > doc;
char base_text[] = "﻿<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n\
<html xmlns=\"http://www.w3.org/1999/xhtml\">\n\
\n\
<head>\n\
	<link rel=\"stylesheet\" style=\"text/css\" href=\"/style.css\">\n\
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
.button:hover {\n\
	background-color: blueviolet\n\
}\n\
\n\
.button:active {\n\
  background-color: #3e8e41;\n\
  box-shadow: 0 5px #666;\n\
  transform: translateY(4px);\n\
}\n\
\n\
.auto-style1 {\n\
	font-family: \"Gill Sans\", \"Gill Sans MT\", Calibri, \"Trebuchet MS\", sans-serif;\n\
	font-size: xx-large;\n\
	color: #FFFFFF;\n\
}\n\
.auto-style2 {\n\
	color: #FFFFFF;\n\
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
  background: darkred;\n\
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
<p class=\"auto-style1\" align=\"center\"><strong>Rikatronic V</strong></p>\n\
<table class=\"auto-style3\" style=\"width: 100%\">\n\
	<tr>\n\
		<td class=\"auto-style2\" style=\"width: 484px\"><strong>Ofentemperatur</strong></td>\n\
		<td class=\"auto-style2\"><strong id=\"temp\"></strong></td>\n\
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
		<td id=\"flapColumn\" class=\"auto-style2\"><strong id=\"flap\"></strong></td>\n\
	</tr>\n\
	<tr>\n\
		<td class=\"auto-style2\" style=\"width: 484px\"><strong>Modus</strong></td>\n\
		<td class=\"auto-style2\"><strong id=\"state\"></strong></td>\n\
	</tr>\n\
</table>\n\
</p>\n\
<table class=\"auto-style2\" style=\"width: 100%\">\n\
	<tr>\n\
		<td>\n\
			<form>\n\
				<p class=\"button\" type=\"submit\" onclick=\"power()\">POWER</p>\n\
			</form>\n\
		</td>\n\
		<td>\n\
			<form >\n\
				<p class=\"button\" type=\"submit\" onclick=\"manual()\">MANUAL</p>\n\
			</form>\n\
		</td>\n\
		<td>\n\
			<p>\n\
				<p class=\"button\" type=\"submit\" onclick=\"eco()\">ECO</p>\n\
			</form>\n\
		</td>\n\
		<td>\n\
			<p>\n\
				<p class=\"button\" type=\"submit\" onclick=\"refill()\">nachlegen</p>\n\
			</form>\n\
		</td>\n\
	</tr>\n\
</table>\n\
</p>\n\
<table  class=\"auto-style2\" style=\"width: 100%\">\n\
	<tr>\n\
		<div class=\"slidecontainer\">\n\
			<input id=\"slider\" type=\"range\" min=\"1\" max=\"100\" value={{ flap }} class=\"slider\" id=\"flap\" onchange=\"sliderChange()\">\n\
		</div>\n\
	</tr>\n\
</table>\n\
</p>\n\
<form action=\"submit\" method=\"get\" >\n\
	<button class=\"button\" id=\"submitBtn\" type=\"submit\" name=\"action\" value=\"{{ json }}\" >übernehmen</button>\n\
</form>\n\
\n\
</body>\n\
\n\
</html>\n\
<script>\n\
\n\
	var values = {\n\
		\"state\" : \"0\",              \n\
		\"flap\" : \"0\",               \n\
		\"temp\" : \"0\",               \n\
		\"program\" : \"0\",            \n\
		\"duration\" : \"0\",           \n\
		\"calibrated\" : \"0\"}         \n\
		\n\
	document.getElementById(\"flap\").innerHTML = values.flap + \"%\"\n\
	document.getElementById(\"slider\").value = values.flap\n\
	document.getElementById(\"temp\").innerHTML = values.temp + \"°C\"\n\
	document.getElementById(\"state\").innerHTML = values.state\n\
	document.getElementById(\"duration\").innerHTML = values.duration\n\
	document.getElementById(\"program\").innerHTML = values.program\n\
	document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
\n\
	function refill()\n\
	{\n\
		values.program = \"REFILL\"\n\
		document.getElementById(\"state\").innerHTML = values.state\n\
		document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
	}\n\
	function power()\n\
	{\n\
		values.state = \"POWER\"\n\
		document.getElementById(\"state\").innerHTML = values.state\n\
		document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
	}\n\
	function eco()\n\
	{\n\
		values.state = \"ECO\"\n\
		document.getElementById(\"state\").innerHTML = values.state\n\
		document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
	}\n\
	function manual()\n\
	{\n\
		values.state = \"MANUAL\"\n\
		document.getElementById(\"state\").innerHTML = values.state\n\
		document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
	}\n\
	function sliderChange()\n\
	{\n\
		if (\"MANUAL\" == values.state)\n\
		{\n\
			values.flap = document.getElementById(\"slider\").value\n\
			document.getElementById(\"flap\").innerHTML = values.flap + \"% (übernehmen)\"\n\
			document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
		}\n\
		else\n\
		{\n\
			document.getElementById(\"slider\").value = values.flap\n\
		}\n\
	}\n\
\n\
</script>";

base::base(ESP8266WebServer* server)
{
	this->server = server;
	this->submit_UserCallback = (void(*)()) NULL;
	this->server->on("/submit", std::bind( & base::Submit_Callback, this));
}
void base::Submit_Callback(void)
{
	String jsonstring = this->server->arg("action");
	DeserializationError error = deserializeJson(doc, jsonstring);
	Serial.println(jsonstring);
	JsonObject obj = doc.as < JsonObject > ();

	if (error)
	{
	Serial.print(F("deserializeJson() failed: "));
	Serial.println(error.f_str());
	}
	else
	{
		this->program = obj["program"].as < String > ();
		this->duration = obj["duration"].as < String > ();
		this->temp = obj["temp"].as < String > ();
		this->calibrated = obj["calibrated"].as < String > ();
		this->state = obj["state"].as < String > ();
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

void base::Set_program (String value)
{
	this->program = value;
	this->Replace("program", this->program);
}

String base::Get_program ( void )
{
	return this->program;
}
void base::Set_duration (String value)
{
	this->duration = value;
	this->Replace("duration", this->duration);
}

String base::Get_duration ( void )
{
	return this->duration;
}
void base::Set_temp (String value)
{
	this->temp = value;
	this->Replace("temp", this->temp);
}

String base::Get_temp ( void )
{
	return this->temp;
}
void base::Set_calibrated (String value)
{
	this->calibrated = value;
	this->Replace("calibrated", this->calibrated);
}

String base::Get_calibrated ( void )
{
	return this->calibrated;
}
void base::Set_state (String value)
{
	this->state = value;
	this->Replace("state", this->state);
}

String base::Get_state ( void )
{
	return this->state;
}
void base::Set_flap (String value)
{
	this->flap = value;
	this->Replace("flap", this->flap);
}

String base::Get_flap ( void )
{
	return this->flap;
}
void base::Render( void )
{
	this->server->send( 200, base_text );
}
void base::Replace(String var, String val)
{
	int varLength = var.length() + 1;
	int valLength = val.length() + 1;
	char varName[varLength];
	char value[valLength];
	char tmpVarName[varLength];
	if (10 < valLength)
	{
		valLength = 10;
	}
	var.toCharArray(varName, varLength);
	val.toCharArray(value, valLength);
#ifdef DEBUG
	 Serial.println("Search for " + var + ");
#endif
	for (int i=0; i < sizeof(base_text); i++)
	{
		if ('\n' == base_text[i-1])
		{
			memcpy(tmpVarName, & base_text[i+3], varLength);
			tmpVarName[varLength - 1] = '\0';
			if ((0 == strcmp(varName, tmpVarName)) && ('\"' == base_text[i+2]) && ('\"' == base_text[i+2+varLength]))
			{
				/* replace variable and set iterator to end of line */
				i = this->ReplaceJSVariable((i+7+varLength), varName, value, valLength);
			}
		}
	}
}
int base::ReplaceJSVariable(int index, const char * varName, const char * value, int valLength)
{
	char lastChar = ' ';
	int endOfLine = index;
#ifdef DEBUG
	Serial.println(String("Found variable: ") + varName + " first char: " + base_text[index]);
#endif


	/*delete value in line */
	do
	{
		if (' ' != base_text[endOfLine])
		{
			lastChar = base_text[endOfLine];
		}
		base_text[endOfLine] = ' ';
		endOfLine ++;
	}while (base_text[endOfLine] != '\n');
#ifdef DEBUG
	Serial.println("Value deleted");
#endif
	memcpy( &base_text[index], value, (valLength - 1));
	base_text[(index + valLength - 1)] = '\"';
	base_text[(index + valLength)] = lastChar;
	return endOfLine;
}
