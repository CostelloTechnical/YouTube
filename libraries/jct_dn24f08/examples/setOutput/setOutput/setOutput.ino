#include <jct_dn24f08.h>

dn24f08 plc;

void setup() {
  plc.init();
  // The channel can be either in the CH1..CH8 or 1...8 formats.
  plc.setOutput(CH1, true);
  plc.setOutput(2, true);
}

void loop() {
  // The engineDisplay must be run to update the channels set.
  // This is because the seven segment display and the digital outputs are operated by the same set of 74HC595D ICs.
  plc.engineDisplay();
}