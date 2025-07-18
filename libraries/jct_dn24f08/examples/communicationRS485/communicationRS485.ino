#include <jct_dn24f08.h>

dn24f08 plc;

void setup() {
  plc.init();
  plc.setCommunicationConfiguration(Serial, 115200, '<', '>');
}

void loop() {
  plc.checkCommunication();

  if (plc.getDataReady() == true) {
    plc.println(plc.getReceivedCharacters());
  }
}