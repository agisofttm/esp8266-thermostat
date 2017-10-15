#include "serialControl.h"

SerialControl::SerialControl() {
    command = "";
    receivedCommand = false;

}

void SerialControl::setup() {
    Serial.begin(9600);
    command.reserve(200);
}



void SerialControl::write(String s) {
    Serial.println(s);
}

void SerialControl::onData() {
    while (Serial.available() && !receivedCommand) {
        char inChar = (char)Serial.read();
        //Serial.println("char:"+String(inChar));
        if (inChar == '\n') {
            receivedCommand = true;
        } else {
            command += inChar;
        }
    }
}

boolean SerialControl::received() {
    return receivedCommand;
}

String SerialControl::getCommand() {
    receivedCommand = false;
    String commandToReturn = command;
    command = "";
    return commandToReturn;
}


