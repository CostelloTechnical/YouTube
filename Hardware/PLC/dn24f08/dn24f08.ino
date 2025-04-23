/*
The nano board must be 5V logic compatable.

Key 1 connected to pin 5
Key 2 connected to pin 6
Key 3 connected to pin 7
Key 4 connected to pin 8

I1 is connected to pin A0 (0-20mA)
I2 is connected to pin A1 (0-20mA)
I3 is connected to pin A2 (0-20mA)
I4 is connected to pin A3 (0-20mA)

V1 is connected to pin A4 (0-10V)
V1 is connected to pin A5 (0-10V)
V1 is connected to pin A6 (0-10V)
V1 is connected to pin A7 (0-10V)

The 8 inputs are on a 74HC165
  Data = pin 2
  Clock = pin 3
  Load = pin 4

The 8 outputs and 7 segment display are controlled with 3 74HC595D ICs in series:
  Data = pin 9
  Output enable = pin 10
  Latch = pin 11
  Clock = pin 12

  Layout of pins:
  595_1 = {A,B,C,D,E,F,G,DP}; These are the LED character segment constructers.
  595_2 = {G1,G2,G3,G4,NC,NC,NC,NC}; The are the commons for each segment cluster.
  595_3 = {J1,J2,J3,J4,J5,J6,J7,J8}; These are the outputs.

  For the 7seg with MSBF, 1 disables the leftmost display.

Communication is done via RS485:
  RX = pin 0
  TX = pin 1
  Recieve/Transmit = pin 13
*/
#include <dn24f08.h>
#include <jct_serial.h>
dn24f08 plc;
jctSerial rs485;
bool sequence = 0;
uint8_t sequenceIterator = 1;
uint32_t sequenceTime_ms = 0;

char debug[254];
bool state = false;

char message[100];
char action[4];
char type[3];
uint8_t pin;
char value[10];

void setup() {
  plc.init();
  rs485.init(Serial, 115200, 13);
  plc.setAnalogCalibration(V1, 1.1029, 0.0459);
}

void loop() {
  plc.engineDisplay();
  rs485.check();
  if (rs485.getDataReady()) {
    message[0] = '\0';
    action[0] = '\0';
    type[0] = '\0';
    pin = 255;
    value[0] = '\0';
    strcpy(message, rs485.getReceivedCharacters());
    strcpy(action, strtok(message, ","));
    strcpy(type, strtok(NULL, ","));
    pin = atoi(strtok(NULL, ","));
    if (strcmp(action, "set") == 0) {
      strcpy(value, strtok(NULL, ","));
    }
    if (strcmp(type, "DO") == 0) {
      if (strcmp(action, "set") == 0) {
        plc.setOutput(pin, (bool)atoi(value));
      }  //
      else if (strcmp(action, "get") == 0) {
        sprintf(value, "%d", plc.getOutput(pin));
      }
    }  //
    else if (strcmp(type, "DI") == 0) {
      if (strcmp(action, "get") == 0) {
        sprintf(value, "%d", plc.getInput(pin));
      }  //
      else if (strcmp(action, "set") == 0) {
        strcpy(value, "NA");
        ;
      }  //
      else {
        strcpy(value, "ERROR");
        ;
      }
    }  //
    else if (strcmp(type, "AI") == 0) {
      if (strcmp(action, "get") == 0) {
        dtostrf(plc.getAnalog(pin), 0, 4, value);
      }  //
      else if (strcmp(action, "set") == 0) {
        strcpy(value, "NA");
        ;
      }  //
      else {
        strcpy(value, "ERROR");
        ;
      }
    }  //
    else if (strcmp(type, "DT") == 0) {
      if (strcmp(action, "set") == 0) {
        plc.setDisplayEngineType(atoi(value));
      }  //
      else {
        strcpy(value, "ERROR");
        ;
      }
    }  //
    else if (strcmp(type, "DP") == 0) {
      if (strcmp(action, "set") == 0) {
        plc.setDisplayAnalogPin(pin);
      }  //
      else {
        strcpy(value, "ERROR");
        ;
      }
    }  //
    else if (strcmp(type, "DN") == 0) {
      if (strcmp(action, "set") == 0) {
        plc.setDisplayInteger(atoi(value));
      }  //
      else {
        strcpy(value, "ERROR");
        ;
      }
    }  //
    else if (strcmp(type, "SQ") == 0) {
      if (strcmp(action, "set") == 0) {
        sequence = (bool)atoi(value);
      }  //
      else {
        strcpy(value, "ERROR");
        ;
      }
    }
    sprintf(message, "<%s,%s,%d,%s>", action, type, pin, value);
    rs485.print(message);
  }
  if (sequence == true) {
    if (sequenceIterator < 5) {
      if (millis() - sequenceTime_ms > 500) {
        plc.setOutput(sequenceIterator, !plc.getOutput(sequenceIterator));
        sequenceIterator++;
        sequenceTime_ms = millis();
      }
    }  //
    else {
      state = !state;
      sequenceIterator = 1;
    }
  }
}
