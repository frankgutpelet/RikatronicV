#include "base.hpp"
#include <ArduinoJson.h>

StaticJsonDocument < 600 > base_doc;
const char base_text[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="de">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta name="theme-color" content="#0b1020">
<title>Rikatronic V</title>
<style>
:root {
	--bg: #0b1020; --card: #141b2e; --card2: #1b243b; --line: #27324d;
	--text: #e8ecf6; --mute: #8b97b5;
	--accent: #ff8a3d; --accent2: #ffb36b;
	--ok: #34d399; --warn: #fbbf24; --bad: #f87171;
}
@media (prefers-color-scheme: light) {
	:root {
		--bg: #f3f5fa; --card: #ffffff; --card2: #eef1f8; --line: #dde3f0;
		--text: #161c2e; --mute: #667290;
		--accent: #f06a0f; --accent2: #ff9a4d;
		--ok: #059669; --warn: #b45309; --bad: #dc2626;
	}
}
* { box-sizing: border-box; }
body {
	margin: 0; min-height: 100vh; color: var(--text);
	font: 16px/1.4 system-ui, -apple-system, "Segoe UI", Roboto, sans-serif;
	background: radial-gradient(900px 500px at 15% -10%, rgba(255,138,61,.14), transparent 60%), var(--bg);
}
.wrap { max-width: 560px; margin: 0 auto; padding: 20px 16px 40px; }
header { display: flex; align-items: center; justify-content: space-between; gap: 12px; margin-bottom: 18px; }
h1 { margin: 0; font-size: 1.35rem; font-weight: 700; letter-spacing: .2px; }
.ver { color: var(--mute); font-size: .8rem; margin-top: 2px; }
.card { background: var(--card); border: 1px solid var(--line); border-radius: 20px; padding: 20px; margin-bottom: 16px; }
.card h2 { margin: 0 0 14px; font-size: .8rem; font-weight: 600; text-transform: uppercase; letter-spacing: .08em; color: var(--mute); display: flex; justify-content: space-between; align-items: center; }
.pill { display: inline-flex; align-items: center; gap: 6px; font-size: .8rem; font-weight: 600; padding: 4px 10px; border-radius: 99px; background: var(--card2); color: var(--mute); text-transform: none; letter-spacing: 0; }
.pill::before { content: ""; width: 8px; height: 8px; border-radius: 50%; background: currentColor; }
.pill.ok { color: var(--ok); } .pill.warn { color: var(--warn); } .pill.bad { color: var(--bad); }

.temp { font-size: 3.4rem; font-weight: 700; line-height: 1.1; } .temp small { font-size: 1.3rem; color: var(--mute); }
.hint { color: var(--mute); font-size: .85rem; margin-top: 4px; min-height: 1.2em; }

.seg { display: flex; background: var(--card2); border: 1px solid var(--line); border-radius: 14px; padding: 4px; gap: 4px; margin-top: 18px; }
.seg button { flex: 1; border: none; background: transparent; padding: 10px 0; border-radius: 10px; color: var(--mute); }
.seg button.on { background: linear-gradient(135deg, var(--accent2), var(--accent)); color: #1a0e02; }

.gauge { position: relative; width: 220px; height: 220px; margin: 0 auto 6px; }
.gauge svg { width: 100%; height: 100%; }
.gauge .track { stroke: var(--card2); }
.gauge .bar { stroke: url(#grad); transition: stroke-dasharray .25s ease; }
.gauge .center { position: absolute; inset: 0; display: flex; flex-direction: column; align-items: center; justify-content: center; }
.gauge .num { font-size: 3.4rem; font-weight: 700; line-height: 1; }
.gauge .num small { font-size: 1.3rem; color: var(--mute); margin-left: 2px; }
.gauge .lbl { color: var(--mute); font-size: .85rem; margin-top: 6px; }

input[type=range] { -webkit-appearance: none; appearance: none; width: 100%; height: 12px; border-radius: 99px; outline: none; margin: 14px 0 18px;
	background: linear-gradient(90deg, var(--accent2), var(--accent)) 0 / var(--p, 0%) 100% no-repeat, var(--card2); }
input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; width: 30px; height: 30px; border-radius: 50%; background: #fff; border: 4px solid var(--accent); box-shadow: 0 2px 8px rgba(0,0,0,.35); cursor: pointer; }
input[type=range]::-moz-range-thumb { width: 22px; height: 22px; border-radius: 50%; background: #fff; border: 4px solid var(--accent); cursor: pointer; }
input[type=range]:disabled { opacity: .45; }
input[type=range]:disabled::-webkit-slider-thumb { cursor: not-allowed; }

.row { display: flex; gap: 10px; }
button { font: inherit; font-weight: 600; cursor: pointer; border: 1px solid var(--line); background: var(--card2); color: var(--text); border-radius: 14px; padding: 12px 16px; flex: 1; transition: transform .05s, filter .15s; }
button:hover { filter: brightness(1.12); } button:active { transform: scale(.97); }
button:disabled { opacity: .45; cursor: not-allowed; filter: none; transform: none; }
button.primary { background: linear-gradient(135deg, var(--accent2), var(--accent)); border: none; color: #1a0e02; width: 100%; margin-top: 18px; }
button.off { color: var(--bad); }

label { display: block; font-size: .85rem; color: var(--mute); margin: 12px 0 5px; }
input[type=text], input[type=password], input[type=number] { width: 100%; font: inherit; color: var(--text); background: var(--card2); border: 1px solid var(--line); border-radius: 12px; padding: 9px 10px; outline: none; }
input:focus { border-color: var(--accent); }
.grid { display: grid; grid-template-columns: 1fr 110px; gap: 10px; }

.switch { display: flex; align-items: center; justify-content: space-between; margin: 0; font-size: 1rem; color: var(--text); }
.switch input { appearance: none; -webkit-appearance: none; width: 50px; height: 30px; border-radius: 99px; background: var(--card2); border: 1px solid var(--line); position: relative; cursor: pointer; transition: background .2s; flex: none; }
.switch input::after { content: ""; position: absolute; top: 3px; left: 3px; width: 22px; height: 22px; border-radius: 50%; background: #fff; transition: transform .2s; }
.switch input:checked { background: var(--accent); border-color: var(--accent); }
.switch input:checked::after { transform: translateX(20px); }
.beeprow { display: flex; align-items: center; gap: 14px; }
.beeprow .switch { flex: 1; }
.beeprow button { flex: none; padding: 9px 16px; }

details { margin-top: 4px; } summary { cursor: pointer; color: var(--mute); font-size: .9rem; padding: 4px 0; }
pre { white-space: pre-wrap; margin: 10px 0 0; padding: 12px; background: var(--card2); border-radius: 12px; font-size: .8rem; color: var(--text); }

#toast { position: fixed; left: 50%; bottom: 24px; transform: translate(-50%, 80px); background: var(--text); color: var(--bg); padding: 10px 18px; border-radius: 99px; font-weight: 600; font-size: .9rem; opacity: 0; transition: all .25s; pointer-events: none; }
#toast.show { transform: translate(-50%, 0); opacity: 1; }
</style>
</head>
<body>
<div class="wrap">
	<header>
		<div>
			<h1 id="headline">Rikatronic V</h1>
			<div class="ver" id="version"></div>
		</div>
		<span class="pill" id="conn">verbinde&hellip;</span>
	</header>

	<section class="card">
		<h2>Ofentemperatur <span class="pill" id="prog">--</span></h2>
		<div class="temp"><span id="temp">--</span><small> &deg;C</small></div>
		<div class="hint" id="tempHint"></div>
		<div class="seg" id="modeSeg">
			<button data-mode="MANUAL" onclick="setMode('MANUAL')">Manuell</button>
			<button data-mode="ECO" onclick="setMode('ECO')">Eco +</button>
			<button data-mode="POWER" onclick="setMode('POWER')">Power</button>
		</div>
	</section>

	<section class="card">
		<h2>Klappe</h2>
		<div class="gauge">
			<svg viewBox="0 0 120 120">
				<defs>
					<linearGradient id="grad" x1="0" y1="1" x2="1" y2="0">
						<stop offset="0" stop-color="#ffb36b"/><stop offset="1" stop-color="#ff5d3d"/>
					</linearGradient>
				</defs>
				<circle class="track" cx="60" cy="60" r="52" fill="none" stroke-width="10" stroke-linecap="round" stroke-dasharray="245 327" transform="rotate(135 60 60)"/>
				<circle class="bar" id="bar" cx="60" cy="60" r="52" fill="none" stroke-width="10" stroke-linecap="round" stroke-dasharray="0 327" transform="rotate(135 60 60)"/>
			</svg>
			<div class="center">
				<div class="num"><span id="flapNum">0</span><small>%</small></div>
				<div class="lbl" id="flapLbl">Automatik</div>
			</div>
		</div>
		<input type="range" min="0" max="100" value="0" step="1" id="flapSlider" disabled>
		<div class="row">
			<button id="bMinus" onclick="step(-5)" disabled>&minus; 5</button>
			<button id="bZu" class="off" onclick="sendFlap(0)" disabled>Zu</button>
			<button id="bPlus" onclick="step(5)" disabled>+ 5</button>
		</div>
		<div class="hint" id="flapHint">Die Klappe l&auml;sst sich nur im Modus &bdquo;Manuell&ldquo; von Hand einstellen.</div>
		<button class="primary" onclick="refill()">Nachlegen</button>
	</section>

	<section class="card">
		<h2>Signalton</h2>
		<div class="beeprow">
			<label class="switch" style="margin:0">Piepen bei Bedarf <input type="checkbox" id="beepSw" onchange="setBeep(this.checked)"></label>
			<button onclick="testBeep()">Test</button>
		</div>
	</section>

	<section class="card">
		<h2>MQTT <span class="pill" id="mqttPill">aus</span></h2>
		<form id="mqttForm" onsubmit="return saveMqtt(event)">
			<label class="switch">MQTT aktivieren <input type="checkbox" id="mEnabled"></label>
			<label for="mHost">Broker</label>
			<div class="grid">
				<input type="text" id="mHost" placeholder="192.168.1.10" autocomplete="off" maxlength="63">
				<input type="number" id="mPort" placeholder="1883" min="1" max="65535">
			</div>
			<label for="mUser">Benutzer (optional)</label>
			<input type="text" id="mUser" autocomplete="off" maxlength="31">
			<label for="mPass">Passwort (leer = unver&auml;ndert)</label>
			<input type="password" id="mPass" autocomplete="new-password" maxlength="63">
			<label for="mTopic">Basis-Topic</label>
			<input type="text" id="mTopic" placeholder="ofen" autocomplete="off" maxlength="47" oninput="topics()">
			<button class="primary" type="submit">Speichern</button>
		</form>
		<details>
			<summary>Schnittstellen</summary>
			<pre id="api"></pre>
		</details>
	</section>
</div>
<div id="toast"></div>
</body>
</html>

<script>
	var values =
	{
		"state" : "dynamic",
		"flap" : "dynamic",
		"temp" : "dynamic",
		"program" : "dynamic",
		"beep" : "dynamic",
		"version" : "dynamic",
		"headline" : "dynamic",
		"mqtt" : "dynamic",
	}
	var editUntil = 0
	var sendTimer = null
	function $(id) { return document.getElementById(id) }

	function toast(msg)
	{
		var t = $("toast")
		t.textContent = msg
		t.classList.add("show")
		clearTimeout(t.timer)
		t.timer = setTimeout(function() { t.classList.remove("show") }, 2200)
	}

	function setPill(el, cls, text)
	{
		el.className = "pill " + cls
		el.textContent = text
	}

	function post(obj)
	{
		editUntil = Date.now() + 3000
		return fetch("submit", {
			method: "POST",
			headers: {"Content-Type": "application/x-www-form-urlencoded"},
			body: "action=" + encodeURIComponent(JSON.stringify(obj))
		}).then(function(r)
		{
			if (r.status == 400) throw new Error("Nicht möglich in diesem Modus")
			if (!r.ok) throw new Error("Senden fehlgeschlagen")
			editUntil = 0
			updateValues()
		}).catch(function(e)
		{
			editUntil = 0
			toast(e.message || "Senden fehlgeschlagen")
			updateValues()
		})
	}

	function setMode(m) { post({"state": m}) }
	function refill() { post({"program": "REFILL"}).then(function() { toast("Nachlegen gestartet") }) }
	function setBeep(on) { post({"beep": on ? "ON" : "OFF"}) }
	function testBeep() { post({"beep": "TEST"}) }

	function showFlap(v)
	{
		v = Math.max(0, Math.min(100, v))
		$("flapNum").textContent = v
		$("flapSlider").value = v
		$("flapSlider").style.setProperty("--p", v + "%")
		$("bar").style.strokeDasharray = (245 * v / 100) + " 327"
		$("bar").style.opacity = v > 0 ? 1 : 0
	}

	function sendFlap(v)
	{
		v = Math.max(0, Math.min(100, Math.round(v)))
		editUntil = Date.now() + 2500
		showFlap(v)
		clearTimeout(sendTimer)
		sendTimer = setTimeout(function() { post({"flap": String(v)}) }, 150)
	}

	function step(d) { sendFlap(parseInt($("flapSlider").value) + d) }

	$("flapSlider").addEventListener("input", function() { sendFlap(parseInt(this.value)) })

	var progClass = {"Anheizen": "warn", "Automatik": "ok", "Erhalten": "ok", "Nachlegen": "warn", "Rettung": "bad"}

	function show()
	{
		document.title = values.headline
		$("headline").textContent = values.headline
		$("version").textContent = values.version ? "Version " + values.version : ""

		var t = parseInt(values.temp)
		$("temp").textContent = isNaN(t) ? "--" : t
		$("tempHint").textContent = ""
		setPill($("prog"), progClass[values.program] || "", values.program || "--")

		var manual = (values.state == "MANUAL")
		var btns = $("modeSeg").children
		for (var i = 0; i < btns.length; i++)
		{
			btns[i].classList.toggle("on", btns[i].getAttribute("data-mode") == values.state)
		}
		$("flapSlider").disabled = !manual
		$("bMinus").disabled = !manual
		$("bPlus").disabled = !manual
		$("bZu").disabled = !manual
		$("flapLbl").textContent = manual ? "manuell" : "Automatik"
		$("flapHint").style.visibility = manual ? "hidden" : "visible"

		if (Date.now() > editUntil)
		{
			showFlap(parseInt(values.flap) || 0)
			$("beepSw").checked = (values.beep == "ON")
		}

		var m = values.mqtt
		if (m == "verbunden") setPill($("mqttPill"), "ok", "verbunden")
		else if (m == "getrennt") setPill($("mqttPill"), "warn", "getrennt")
		else setPill($("mqttPill"), "", "aus")
	}

	function updateValues()
	{
		fetch("getValues", {cache: "no-store"}).then(function(r) { return r.text() }).then(function(text)
		{
			text.split(";").forEach(function(pair)
			{
				var i = pair.indexOf(":")
				if (i > 0) values[pair.slice(0, i)] = pair.slice(i + 1)
			})
			setPill($("conn"), "ok", "online")
			show()
		}).catch(function()
		{
			setPill($("conn"), "bad", "offline")
		})
	}

	function topics()
	{
		var t = $("mTopic").value || "ofen"
		$("api").textContent =
			"MQTT lesen (retained)\n" +
			"  " + t + "/temp       Ofentemperatur in °C\n" +
			"  " + t + "/flap       Klappe in %\n" +
			"  " + t + "/mode       manual / eco / power\n" +
			"  " + t + "/program    Programm (Anheizen, Automatik, ...)\n" +
			"  " + t + "/beep       ON / OFF\n" +
			"  " + t + "/status     online / offline\n\n" +
			"MQTT setzen (<name>/set)\n" +
			"  " + t + "/mode/set    manual / eco / power\n" +
			"  " + t + "/flap/set    0-100 (nur im Modus manual)\n" +
			"  " + t + "/beep/set    ON / OFF / TEST\n" +
			"  " + t + "/refill/set  beliebig (au\u00dfer 0/OFF) = Nachlegen starten\n" +
			"  " + t + "/heatup/set  beliebig (au\u00dfer 0/OFF) = Anheizen starten (nicht im Modus manual)\n\n" +
			"Taste am Ofen: kurz = Nachlegen, 3 s halten = Anheizen"
	}

	function fillMqtt(c)
	{
		$("mEnabled").checked = c.enabled
		$("mHost").value = c.host
		$("mPort").value = c.port
		$("mUser").value = c.user
		$("mPass").value = ""
		$("mPass").placeholder = c.hasPass ? "•••••• gespeichert" : ""
		$("mTopic").value = c.topic
		topics()
	}

	function loadMqtt()
	{
		fetch("mqtt", {cache: "no-store"}).then(function(r) { return r.json() }).then(fillMqtt).catch(function() { setTimeout(loadMqtt, 2000) })
	}

	function saveMqtt(ev)
	{
		ev.preventDefault()
		var p = new URLSearchParams()
		p.set("enabled", $("mEnabled").checked ? "1" : "0")
		p.set("host", $("mHost").value.trim())
		p.set("port", $("mPort").value)
		p.set("user", $("mUser").value)
		p.set("pass", $("mPass").value)
		p.set("topic", $("mTopic").value.trim())
		fetch("mqtt", {
			method: "POST",
			headers: {"Content-Type": "application/x-www-form-urlencoded"},
			body: p.toString()
		}).then(function(r)
		{
			if (!r.ok) return r.text().then(function(t) { throw new Error(t) })
			return r.json()
		}).then(function(c)
		{
			fillMqtt(c)
			toast("MQTT gespeichert")
		}).catch(function(e)
		{
			toast(e.message || "Speichern fehlgeschlagen")
		})
		return false
	}

	topics()
	loadMqtt()
	updateValues()
	setInterval(updateValues, 1000)
</script>
)rawliteral";

base::base(ESP8266WebServer* server)
{
	this->server = server;
	this->submit_UserCallback = (void(*)()) NULL;
	this->server->on("/submit", HTTP_POST, std::bind( & base::Submit_Callback, this));
	this->server->on("/getValues", std::bind( & base::GetAjaxValues, this));
}
void base::Submit_Callback(void)
{
	String jsonstring = this->server->arg("action");
	DeserializationError error = deserializeJson(base_doc, jsonstring);
	JsonObject obj = base_doc.as < JsonObject > ();

	if (error)
	{
	Serial.print(F("deserializeJson() failed: "));
	Serial.println(error.f_str());
	}
	else
	{
		if (obj.containsKey("state"))
		{
			this->state = obj["state"].as < String > ();
		}
		if (obj.containsKey("flap"))
		{
			this->flap = obj["flap"].as < String > ();
		}
		if (obj.containsKey("temp"))
		{
			this->temp = obj["temp"].as < String > ();
		}
		if (obj.containsKey("program"))
		{
			this->program = obj["program"].as < String > ();
		}
		if (obj.containsKey("beep"))
		{
			this->beep = obj["beep"].as < String > ();
		}
		if (obj.containsKey("version"))
		{
			this->version = obj["version"].as < String > ();
		}
		if (obj.containsKey("headline"))
		{
			this->headline = obj["headline"].as < String > ();
		}
		if (obj.containsKey("mqtt"))
		{
			this->mqtt = obj["mqtt"].as < String > ();
		}

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
void base::Set_flap (String value)
{
	this->flap = value;
}

String base::Get_flap ( void )
{
	return this->flap;
}
void base::Set_temp (String value)
{
	this->temp = value;
}

String base::Get_temp ( void )
{
	return this->temp;
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
void base::Set_version (String value)
{
	this->version = value;
}

String base::Get_version ( void )
{
	return this->version;
}
void base::Set_headline (String value)
{
	this->headline = value;
}

String base::Get_headline ( void )
{
	return this->headline;
}
void base::Set_mqtt (String value)
{
	this->mqtt = value;
}

String base::Get_mqtt ( void )
{
	return this->mqtt;
}
void base::Render( void )
{
	this->server->send_P( 200, "text/html; charset=utf-8", base_text);
}
void base::GetAjaxValues( void )
{
	String message = "state:" + this->state + ";flap:" + this->flap + ";temp:" + this->temp + ";program:" + this->program + ";beep:" + this->beep + ";version:" + this->version + ";headline:" + this->headline + ";mqtt:" + this->mqtt;
	this->server->send(200, "text/plain", message);
}
