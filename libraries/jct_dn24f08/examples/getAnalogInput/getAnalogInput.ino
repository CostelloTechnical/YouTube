#include <jct_dn24f08.h>

dn24f08 plc;

uint32_t printTimeCache_ms = 0;

void setup() {
  Serial.begin(9600);
  plc.init();

  plc.setAnalogCalibration(V1, 1, 0);
  plc.setAnalogEngineType(TIME_MS, 100);
  plc.getAnalog(V1);
  plc.getAnalog(I1);
}

void loop() {
  plc.engineAnalogAverage();
  if (millis() - printTimeCache_ms > 1000) {
    Serial.println(plc.getAnalogAverage(V1));
    printTimeCache_ms = millis();
  }
}