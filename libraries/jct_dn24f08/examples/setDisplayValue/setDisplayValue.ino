#include <jct_dn24f08.h>

dn24f08 plc;

void setup() {
  plc.init();

  plc.setDisplayEngineType(ANALOG);
  plc.setDisplayAnalogPin(V1);
  plc.setAnalogCalibration(V1, 1.1029, 0.0459);
  plc.setAnalogEngineType(TIME_MS, 100);
}

void loop() {

  plc.engineAnalogAverage();
  plc.engineDisplay();
}