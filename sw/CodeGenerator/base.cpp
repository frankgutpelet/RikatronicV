#include base.hstatic const char[] base_text = "﻿<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

<head>
	<link rel="stylesheet" style="text/css" href="/style.css">
<meta content="de" http-equiv="Content-Language" />
<meta content="text/html; charset=utf-8" http-equiv="Content-Type" />
<title>Rikatronic V</title>
<style type="text/css">



body {
background-color: blue
}

.button {
  display: inline-block;
  padding: 15px 25px;
  font-size: 24px;
  cursor: pointer;
  text-align: center;
  text-decoration: none;
  outline: none;
  color: #fff;
  background-color: silver;
  border: none;
  border-radius: 10px;
  box-shadow: 0 9px darkblue;
}

.button:hover {background-color: #3e8e41}

.button:active {
  background-color: #3e8e41;
  box-shadow: 0 5px #666;
  transform: translateY(4px);
}

.auto-style1 {
	font-family: "Gill Sans", "Gill Sans MT", Calibri, "Trebuchet MS", sans-serif;
	font-size: xx-large;
	color: #FFFFFF;
}
.auto-style2 {
	color: #FFFFFF;
}
.auto-style3 {
	border-style: solid;
	border-width: 1px;
}
.slider {
  -webkit-appearance: none;
  width: 100%;
  height: 35px;
  background: #silver;
  outline: none;
  opacity: 0.7;
  -webkit-transition: .2s;
  transition: opacity .2s;
}
.slider::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 50px;
  height: 35px;
  background: darkred;
  cursor: pointer;
}
</style>
</head>
<p class="auto-style1"><strong>Rikatronic V</strong></p>
<table class="auto-style3" style="width: 100%">
	<tr>
		<td class="auto-style2" style="width: 484px"><strong>Ofentemperatur</strong></td>
		<td class="auto-style2"><strong>{{ temp }} °C</strong></td>
	</tr>
	<tr>
		<td class="auto-style2" style="width: 484px"><strong>Programm</strong></td>
		<td class="auto-style2"><strong>{{ program }}</strong></td>
	</tr>
	<tr>
		<td class="auto-style2" style="width: 484px"><strong>Brennzeit</strong></td>
		<td class="auto-style2"><strong>{{ time }}</strong></td>
	</tr>
	<tr>
		<td class="auto-style2" style="width: 484px"><strong>Klappe</strong></td>
		<td class="auto-style2"><strong>{{ flap }} %</strong></td>
	</tr>
	<tr>
		<td class="auto-style2" style="width: 484px"><strong>Modus</strong></td>
		<td class="auto-style2"><strong>{{ mode }}</strong></td>
	</tr>
</table>
</p>
<table class="auto-style2" style="width: 100%">
	<tr>
		<td>
			<form action="/Rikatronic/power" >
				<input class="button" type="submit" value="POWER">
			</form>
		</td>
		<td>
			<form action="/Rikatronic/manual" >
    			<input class="button" type="submit" value="MANUAL">
			</form>
		</td>
		<td>
			<form action="/Rikatronic/eco" >
    			<input class="button" type="submit" value="ECO">
			</form>
		</td>
	</tr>
</table>
</p>
<table  class="auto-style2" style="width: 100%">
	<tr>
		<div class="slidecontainer">
			<form action="/Rikatronic/flap" method="get" >
			  <input id="slider" type="range" min="1" max="100" value={{ flap }} class="slider" id="flap" onchange="/Rikatronic/flap">
			</form>
		</div>
	</tr>
</table>

<script >

</script>
</body>

</html>
"

base::base(ESP8266WebServer* server)
{
	this->server = server;
}

void base::Settemp (String value)
{
	this->temp = value;
}

void base::Setprogram (String value)
{
	this->program = value;
}

void base::Settime (String value)
{
	this->time = value;
}

void base::Setflap (String value)
{
	this->flap = value;
}

void base::Setmode (String value)
{
	this->mode = value;
}

void base::Setflap (String value)
{
	this->flap = value;
}

void base::Render( void )
{
	this->server->send( 200, base_text \);
}
