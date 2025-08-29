/*
  This example is to show how to setup and use the serial communication functionality.
  In the uncommented line of the setup the Baud is set to 115200 bps.
  The start character is set to < and the end character is set to >.
  This means that in the serial monitor or serial terminal of your choice (e.g. PuTTY) you set the Baud to 115200.
  To get the below code to print a message just enter "<Your message here>" into and you should receive "Your message here".
  If using the RS485 IC, be sure to change the on-board switch to 485_ON. Otherwise leave it on PRO.
*/

#include <jct_dn24f08.h>

dn24f08 plc;

void setup() {
  plc.init(Serial, 115200, '<', '>'); 
  // plc.init(Serial, 9600, '\n');
}

void loop() {
  plc.engineCommunication();

  if (plc.getDataReady() == true) {
    plc.println(plc.getReceivedCharacters());
  }
}