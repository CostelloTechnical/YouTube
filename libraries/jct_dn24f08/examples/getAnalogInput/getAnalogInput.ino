#include <jct_dn24f08.h>

dn24f08 plc;

uint32_t printTimeCache_ms = 0;

void setup() {
  Serial.begin(9600);
  plc.init();

  // Optional - Sets the gain and offset for a given analog input.
  plc.setAnalogCalibration(V1, 1.1029, 0.0459);

  /* 
    Optional - Only for use with the engineAnalogAverage() function. 
    This set if the averaging should be time based or number of readings based, followed by the time/readings.
    Allowable types: TIME_MS, READINGS.
  */
  plc.setAnalogEngineType(TIME_MS, 100);

  plc.getAnalog(V1);  // Read voltage input 1. (Volts)
  plc.getAnalog(I1);  // Read current input 1. (mAmps)
}

void loop() {
  /* 
    Optional - The engineAnalogAverage() is a non-blocking function that iterates through the analog inputs.
    Below I've put together an example where the most recent analog average is printed to the serial monitor every second.
  */
  plc.engineAnalogAverage();
  if (millis() - printTimeCache_ms > 1000) {
    Serial.println(plc.getAnalogAverage(V1));
    printTimeCache_ms = millis();
  }
}