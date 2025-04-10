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
#include "dn24f08.h"
dn24f08 plc;

void displayCharacters(char* characters) {
}

void setup() {
  Serial.begin(115200);
  plc.init();
  plc.setAnalogCalibration(V1, 1.1029, 0.0459);
}

void loop() {
  //plc.analogAverageReadings(100);
  plc.analogAverageTime(100);
  Serial.println();
}
