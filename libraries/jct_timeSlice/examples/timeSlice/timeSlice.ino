/**
 * @file jct_timeSlice.ino
 * @author CostelloTechnical
 * @brief This piece of code is a demonstration of a potentially useful way to implement a time-slice multithread system.
 * The working principle is that each switch case is a "thread" and each case within it is a line of code to be executed.
 * At the end of each case the case counter is incremented (this is how it knows which line to reenter on) and a thread 
 * allocation time is checked. If the time has been exceeded, the current switch case is exited and the next switch case is 
 * run.
 * 
 * I've tested the simple code below and it appears to be working fine.
 * @date 2025-08-24
*/

const uint8_t pins = 4;                      // Number of pins being used.
uint8_t pins[pins] = { 10, 11, 12, 13 };     // Pins being controlled in this demonstration.
uint8_t threadCase[pins] = { 1, 1, 1, 1 };   // A index to keep tract of the current case.
uint8_t threadCases[pins] = { 2, 2, 2, 2 };  // Number of cases within the thread.
uint32_t threadPeriod_us = 100;              // The amount of time allocated to a thread.
uint32_t threadTime_us;                      // Cache for measuring elapsed time.

bool updateCase(uint8_t &threadCase, uint8_t threadCases) {
  threadCase = (threadCase < threadCases) ? (threadCase + 1) : 1; // Increment the case, if greater than the number of cases return to first case.
  if (micros() - threadTime_us >= threadPeriod_us) {              // Has the allowed thread time been exceeded?
    threadTime_us = micros();                                     // Reset the time cache.
    return true;                                                  // Returns true to break the case fall-through behaviour.
  }
  return false;                                                   // Returns false to continue case fall-through.
}

void setup() {
  for (uint8_t i = 0; i < pins; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], true);
  }
  threadTime_us = micros();
}

void loop() {
  switch (threadCase[0]) {
    case 1:
      digitalWrite(pins[0], true); // Enter the code you want executed here. Can be multiline, but the point here is to keep it brief and quick.
      if (updateCase(threadCase[0], threadCases[0])) break; // This is required and must be placed at the end of every case.
    case 2:
      digitalWrite(pins[0], false);
      if (updateCase(threadCase[0], threadCases[0])) break;
  }
  switch (threadCase[1]) {
    case 1:
      digitalWrite(pins[1], true);
      if (updateCase(threadCase[1], threadCases[1])) break;
    case 2:
      digitalWrite(pins[1], false);
      if (updateCase(threadCase[1], threadCases[1])) break;
  }
  switch (threadCase[2]) {
    case 1:
      digitalWrite(pins[2], true);
      if (updateCase(threadCase[2], threadCases[2])) break;
    case 2:
      digitalWrite(pins[2], false);
      if (updateCase(threadCase[2], threadCases[2])) break;
  }
  switch (threadCase[3]) {
    case 1:
      digitalWrite(pins[3], true);
      if (updateCase(threadCase[3], threadCases[3])) break;
    case 2:
      digitalWrite(pins[3], false);
      if (updateCase(threadCase[3], threadCases[3])) break;
  }
}
