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
    source.write("const char* " + side.name + "_text = \"")
    for line in side.text:
        source.write(line.replace('\"', '\\\"').replace("\n", "\\n\\\n"))
    source.write("\";")
    source.write("\n\n")
    source.write(side.name + "::" + side.name + "(ESP8266WebServer* server)\n{\n\tthis->server = server;\n")
    source.write("}\n\n")

    for cbFunc in side.callbacks:
        source.write("void " + side.name + "::SetCallback_" + cbFunc.replace("/", "_") + " (void (*callback)(void))\n{\n\tthis->server->on(\"/" + cbFunc + "\", callback);\n}\n\n")
        header.write("\t\tvoid SetCallback_" + cbFunc.replace("/", "_") + " (void (*callback)(void));\n")

    for variable in side.vars:
        header.write("\t\tvoid Set_" + variable + " (String value);\n")
        source.write("void " + side.name + "::Set_" + variable + " (String value)\n{\n\tthis->" + variable + " = value;\n}\n\n")

    source.write("void " + side.name + "::Render( void )\n{\n")
    source.write("\tthis->server->send( 200, " + side.name + "_text );\n")
    source.write("}\n")

    header.write("\tprivate:\n")
    header.write("\t\tESP8266WebServer* server;")

    for variable in side.vars:
        header.write("\t\tString " + variable + ";\n")

    header.write("};")

def parseHtml(filename):
    html = open(filename, "r")
    side = htmlSide(re.search("\/([^\.]+)\.html", filename).groups()[0])

    for line in html:
        side.text += line
        match = re.search('\{\{\s*([^\s\}]+)\s*\}\}', line)
        if (None != match):
            side.vars.append(match.groups()[0])

        match = re.search("form\s+action=\"([^\"]+)\"\s+method=\"get\"\s+", line)
        if (None != match):
            side.callbacks.append(match.groups()[0])

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
