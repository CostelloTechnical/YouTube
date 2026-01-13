#include <jct_dn24f08.h>

uint16_t counter = 0;
uint32_t counterTimeCache_ms = 0;

dn24f08 plc;

void setup() {
  plc.init();

  plc.setDisplayEngineType(INTEGER);
}

void loop() {
  plc.setDisplayInteger(counter);
  plc.engineDisplay();

  if (millis() - counterTimeCache_ms > 1000) {
    counterTimeCache_ms = millis();
    counter++;
    if (counter >= 9999) {
      counter = 0;
    }
  }
}