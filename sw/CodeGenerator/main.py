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
    header.write("\nclass " + side.name + "\n")
    header.write("{\n")
    header.write("\tpublic:\n")
    header.write("\t\t" + side.name + " (ESP8266WebServer* server);\n\n")
    header.write("\t\tvoid Render (void);\n")

    source.write("#include \"" + side.name + ".hpp\"\n")
    source.write("#include <ArduinoJson.h>\n")
    source.write("\n")
    source.write("StaticJsonDocument < 300 > doc;\n")

    source.write("char " + side.name + "_text[] = \"")
    for line in side.text:
        source.write(line.replace('\"', '\\\"').replace("\n", "\\n\\\n"))
    source.write("\";")
    source.write("\n\n")

    source.write(side.name + "::" + side.name + "(ESP8266WebServer* server)\n{\n")
    source.write("\tthis->server = server;\n")
    for cbFunc in side.callbacks:
        source.write("\tthis->" + cbFunc + "_UserCallback = (void(*)()) NULL;\n")
    source.write("\tthis->server->on(\"/submit\", std::bind( & base::Submit_Callback, this));\n}\n")
    source.write("void base::Submit_Callback(void)\n{\n")
    source.write("\tString jsonstring = this->server->arg(\"action\");\n")
    source.write("\tDeserializationError error = deserializeJson(doc, jsonstring);\n")
    source.write("\tSerial.println(jsonstring);\n")
    source.write("\tJsonObject obj = doc.as < JsonObject > ();\n\n")
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
        source.write("String base::Get_" + variable + " ( void )\n{\n\treturn this->" + variable + ";\n}\n")
    source.write("void " + side.name + "::Render( void )\n{\n")
    for var in side.vars:
        source.write("\t\tthis->Replace(\"" + var + "\", this->" + var + ");\n")
    source.write("\tthis->server->send( 200, " + side.name + "_text );\n")
    source.write("}\n")

    source.write("void base::Replace(String var, String val)\n{\n")

    source.write("\tint varLength = var.length() + 1;\n")
    source.write("\tint valLength = val.length() + 1;\n")
    source.write("\tchar varName[varLength];\n")
    source.write("\tchar value[valLength];\n")
    source.write("\tchar tmpVarName[varLength];\n")
    source.write("\tvar.toCharArray(varName, varLength);\n")
    source.write("\tval.toCharArray(value, valLength);\n")

    source.write("#ifdef DEBUG\n\t Serial.println(\"Search for " + var + "\");\n#endif\n")
    source.write("\tfor (int i=0; i < sizeof(base_text); i++)\n\t{\n")
    source.write("\t\tif ('\\n' == base_text[i-1])\n\t\t{\n")
    source.write("\t\t\tmemcpy(tmpVarName, & base_text[i+3], varLength);\n")
    source.write("\t\t\ttmpVarName[varLength - 1] = '\\0';\n")
    source.write("\t\t\tif ((0 == strcmp(varName, tmpVarName)) && ('\\\"' == base_text[i+2]) && ('\\\"' == base_text[i+2+varLength]))\n\t\t\t{\n")
    source.write("\t\t\t\t/* replace variable and set iterator to end of line */\n")
    source.write("\t\t\t\ti = this->ReplaceJSVariable((i+7+varLength), varName, value, valLength);\n")
    source.write("\t\t\t}\n\t\t}\n\t}\n}\n")

    source.write("int base::ReplaceJSVariable(int index, const char * varName, const char * value, int valLength)\n{\n")
    source.write("\tchar lastChar = ' ';\n")
    source.write("\tint endOfLine = index;\n")
    source.write("#ifdef DEBUG\n\tSerial.println(String(\"Found variable: \") + varName + \" first char: \" + base_text[index]);\n#endif\n\n")
    source.write("\n\t/*delete value in line */\n")
    source.write("\tdo\n\t{\n")
    source.write("\t\tif (' ' != base_text[endOfLine])\n\t\t{\n")
    source.write("\t\t\tlastChar = base_text[endOfLine];\n\t\t}\n")
    source.write("\t\tbase_text[endOfLine] = ' ';\n")
    source.write("\t\tendOfLine ++;\n")
    source.write("\t}while (base_text[endOfLine] != \'\\n\');\n")

    source.write("#ifdef DEBUG\n\tSerial.println(\"Value deleted\");\n#endif\n")
    source.write("\tmemcpy( &base_text[index], value, (valLength - 1));\n")
    source.write("\tbase_text[(index + valLength - 1)] = \'\\\"\';\n")
    source.write("\tbase_text[(index + valLength)] = lastChar;\n")
    source.write("\treturn endOfLine;\n}\n")


    header.write("\tprivate:\n")
    header.write("\t\tvoid Submit_Callback(void);\n")
    header.write("\t\tint  ReplaceJSVariable(int index, const char * varName, const char * value, int valLength);\n")
    for cbFunc in side.callbacks:
        header.write("\t\tvoid(*" + cbFunc + "_UserCallback)(void);\n")
    header.write("\t\tvoid Replace(String var, String val);\n")
    header.write("\t\tESP8266WebServer* server;\n")
    header.write("\n")
    for variable in side.vars:
        header.write("\t\tString " + variable + ";\n")

    header.write("};")

def parseHtml(filename):
    html = open(filename, "r")
    side = htmlSide(re.search("\/([^\.]+)\.html", filename).groups()[0])

    for line in html:

        match = re.search('\t\t\"([^\"]+)\"\s:\s\"[^\"]+\"', line)
        if (None != match):
            side.vars.append(match.groups()[0])
            line = line.replace("\n", "")
            linelength = len(line)
            if (30 <= linelength):
                raise Exception("increase space foor variables, length > 30")
            for i in range(0, (30 - linelength)):
                line += " "
            line += '\n'

        match = re.search("form\s+action=\"([^\"]+)\"\s+method=\"get\"\s+", line)
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
