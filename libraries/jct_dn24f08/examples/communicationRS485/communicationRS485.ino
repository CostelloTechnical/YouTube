#include <jct_dn24f08.h>

dn24f08 plc;

void setup() {
  plc.init();
  plc.initCommunication(Serial, 9600, '<', '>');
}

void loop() {
  plc.checkCommunication();

  if (plc.getDataReady() == true) {
    plc.println(plc.getReceivedCharacters());
  }
}