#include <jct_dn24f08.h>

dn24f08 plc;

void setup() {
  plc.init(Serial, 9600, '\n');
  // plc.init(Serial, 9600, '<', '>'); 
}

void loop() {
  plc.engineCommunication();

  if (plc.getDataReady() == true) {
    plc.println(plc.getReceivedCharacters());
  }
}