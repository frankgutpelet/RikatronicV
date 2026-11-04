# Erzeugt aus html/<name>.html die Dateien code/main/<name>.hpp und <name>.cpp
# Aufruf aus dem Ordner CodeGenerator:  python main.py
import os
import re

OUT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "Rikatronic", "main")


class htmlSide:
    def __init__(self, name):
        self.name = name
        self.text = str()
        self.vars = list()
        self.callbacks = ["submit"]  # POST /submit (fetch aus dem Script)


htmlsides = list()


def printClass(side):
    header = open(os.path.join(OUT_DIR, side.name + ".hpp"), 'w', encoding="utf-8", newline="\n")
    source = open(os.path.join(OUT_DIR, side.name + ".cpp"), 'w', encoding="utf-8", newline="\n")

    header.write("#include <ESP8266WebServer.h>\n")
    header.write("#include <Arduino.h>\n")
    header.write("#pragma once")
    header.write("\nclass " + side.name + "\n")
    header.write("{\n")
    header.write("\tpublic:\n")
    header.write("\t\t" + side.name + " (ESP8266WebServer* server);\n\n")
    header.write("\t\tvoid Render (void);\n")
    header.write("\t\tvoid GetAjaxValues (void);\n")

    source.write("#include \"" + side.name + ".hpp\"\n")
    source.write("#include <ArduinoJson.h>\n")
    source.write("\n")
    source.write("StaticJsonDocument < 600 > " + side.name + "_doc;\n")

    # Raw-String im Flash (PROGMEM), spart RAM und braucht kein Escaping
    source.write("const char " + side.name + "_text[] PROGMEM = R\"rawliteral(")
    source.write(side.text)
    source.write("</script>\n")
    source.write(")rawliteral\";")
    source.write("\n\n")

    source.write(side.name + "::" + side.name + "(ESP8266WebServer* server)\n{\n")
    source.write("\tthis->server = server;\n")
    for cbFunc in side.callbacks:
        source.write("\tthis->" + cbFunc + "_UserCallback = (void(*)()) NULL;\n")
    source.write("\tthis->server->on(\"/submit\", HTTP_POST, std::bind( & " + side.name + "::Submit_Callback, this));\n")
    source.write("\tthis->server->on(\"/getValues\", std::bind( & " + side.name + "::GetAjaxValues, this));\n}\n")
    source.write("void " + side.name + "::Submit_Callback(void)\n{\n")
    source.write("\tString jsonstring = this->server->arg(\"action\");\n")
    source.write("\tDeserializationError error = deserializeJson(" + side.name + "_doc, jsonstring);\n")
    source.write("\tJsonObject obj = " + side.name + "_doc.as < JsonObject > ();\n\n")
    source.write("\tif (error)\n\t{\n")
    source.write("\tSerial.print(F(\"deserializeJson() failed: \"));\n")
    source.write("\tSerial.println(error.f_str());\n\t}\n")
    source.write("\telse\n\t{\n")
    for var in side.vars:
        source.write("\t\tif (obj.containsKey(\"" + var + "\"))\n")
        source.write("\t\t{\n\t\t\tthis->" + var + " = obj[\"" + var + "\"].as < String > ();\n\t\t}\n")
    source.write("\n")
    source.write("\t}\n")
    source.write("\tif (NULL != this->submit_UserCallback)\n\t{\n")
    source.write("\t\tthis->submit_UserCallback();\n\t}\n")
    source.write("}\n\n")

    for cbFunc in side.callbacks:
        source.write("void " + side.name + "::SetCallback_" + cbFunc.replace("/", "_") + " (void (*callback)(void))\n{\n\tthis->" + cbFunc + "_UserCallback = callback;\n}\n\n")
        header.write("\t\tvoid SetCallback_" + cbFunc.replace("/", "_") + " (void (*callback)(void));\n")

    for variable in side.vars:
        header.write("\t\tvoid Set_" + variable + " (String value);\n")
        header.write("\t\tString Get_" + variable + " ();\n")
        source.write("void " + side.name + "::Set_" + variable + " (String value)\n{\n\tthis->" + variable + " = value;\n}\n\n")
        source.write("String " + side.name + "::Get_" + variable + " ( void )\n{\n\treturn this->" + variable + ";\n}\n")
    source.write("void " + side.name + "::Render( void )\n{\n")
    source.write("\tthis->server->send_P( 200, \"text/html; charset=utf-8\", " + side.name + "_text);\n")
    source.write("}\n")
    source.write("void " + side.name + "::GetAjaxValues( void )\n{\n")
    line = "\tString message = \""
    for variable in side.vars:
        line += variable + ":\" + this->" + variable + " + \";"
    line = line[:-5]
    line += ";\n"
    source.write(line)
    source.write("\tthis->server->send(200, \"text/plain\", message);\n}\n")
    header.write("\tprivate:\n")
    header.write("\t\tvoid Submit_Callback(void);\n")
    for cbFunc in side.callbacks:
        header.write("\t\tvoid(*" + cbFunc + "_UserCallback)(void);\n")
    header.write("\t\tESP8266WebServer* server;\n")
    header.write("\n")
    for variable in side.vars:
        header.write("\t\tString " + variable + ";\n")

    header.write("};")
    header.close()
    source.close()


def parseHtml(filename):
    html = open(filename, "r", encoding="utf-8-sig")
    side = htmlSide(re.search(r"/([^\.]+)\.html", filename).groups()[0])

    for line in html:

        match = re.search('\t\t\"([^\"]+)\"\\s:\\s\"dynamic\"', line)
        if (None != match):
            side.vars.append(match.groups()[0])

        if "</script>" in line:
            continue

        side.text += line

    html.close()
    if ")rawliteral\"" in side.text:
        raise Exception("html enthaelt das Raw-String-Endzeichen )rawliteral\"")
    htmlsides.append(side)
    side.vars = list(dict.fromkeys(side.vars))  # Reihenfolge stabil, ohne Duplikate


def main():
    parseHtml("html/base.html")

    for side in htmlsides:
        printClass(side)


if __name__ == '__main__':
    main()
