# This is a sample Python script.
import re
# Press Umschalt+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.

class htmlSide:
    def __init__(self, name):
        self.name = name
        self.text = str()
        self.vars = list()
        self.callbacks = list()

htmlsides = list()

def printClass(side):
    header = open("../Rikatronic/main/" + side.name + ".hpp", 'w')
    source = open("../Rikatronic/main/" + side.name + ".cpp", 'w')

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

    source.write("const char " + side.name + "_text[] = \"")
    for line in side.text:
        source.write(line.replace('\"', '\\\"').replace("\n", "\\n\\\n"))
    source.write("</script>\\n\\\n")
    source.write("\";")
    source.write("\n\n")

    source.write(side.name + "::" + side.name + "(ESP8266WebServer* server)\n{\n")
    source.write("\tthis->server = server;\n")
    for cbFunc in side.callbacks:
        source.write("\tthis->" + cbFunc + "_UserCallback = (void(*)()) NULL;\n")
    source.write("\tthis->server->on(\"/submit\", std::bind( & " + side.name + "::Submit_Callback, this));\n")
    source.write("\tthis->server->on(\"/getValues\", std::bind( & " + side.name + "::GetAjaxValues, this));\n}\n")
    source.write("void " + side.name + "::Submit_Callback(void)\n{\n")
    source.write("\tString jsonstring = this->server->arg(\"action\");\n")
    source.write("\tDeserializationError error = deserializeJson(" + side.name + "_doc, jsonstring);\n")
    source.write("\tSerial.println(jsonstring);\n")
    source.write("\tJsonObject obj = " + side.name + "_doc.as < JsonObject > ();\n\n")
    source.write("\tif (error)\n\t{\n")
    source.write("\tSerial.print(F(\"deserializeJson() failed: \"));\n")
    source.write("\tSerial.println(error.f_str());\n\t}\n")
    source.write("\telse\n\t{\n")
    for var in side.vars:
        source.write("\t\tthis->" + var + " = obj[\"" + var + "\"].as < String > ();\n")
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
    source.write("\tthis->server->send( 200, " + side.name + "_text);\n")
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

def parseHtml(filename):
    html = open(filename, "r")
    side = htmlSide(re.search("\/([^\.]+)\.html", filename).groups()[0])

    for line in html:

        match = re.search('\t\t\"([^\"]+)\"\s:\s\"dynamic\"', line)
        if (None != match):
            side.vars.append(match.groups()[0])
            line = line.replace("\n", "")
            linelength = len(line)
            if (50 <= linelength):
                raise Exception("increase space for variables, length > 30")
            for i in range(0, (30 - linelength)):
                line += " "
            line += '\n'

        if "</script>" in line:
            continue

        match = re.search("form\s+action=\"([^\"]+)\"\s+method=\"post\"\s+", line)
        if (None != match):
            side.callbacks.append(match.groups()[0])

        side.text += line

    htmlsides.append(side)
    side.vars = list(set(side.vars))


def main():
    # Use a breakpoint in the code line below to debug your script.
    parseHtml("html/base.html")

    for side in htmlsides:
        printClass(side)


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    main()

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
