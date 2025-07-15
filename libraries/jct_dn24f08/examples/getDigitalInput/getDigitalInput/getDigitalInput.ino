#include <jct_dn24f08.h>

dn24f08 plc;

void setup() {
  plc.init();
  // The input can be either in the IN1..IN8 or 1...8 formats.
  plc.getInput(IN1);
  plc.getInput(2);
}

void loop() {

}