#include <Servo.h>
Servo myservo;

const char startCharacter = '<';
const char endCharacter = '>';
const uint8_t maxCharacters = 10;
char receivedCharacters[maxCharacters];
bool receivingData = false;
bool dataReady = false;
uint8_t receivedCharcterIndex = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  if (Serial.available() > 0) {
    char receivedCharacter = Serial.read();

    if (receivedCharacter == startCharacter) {
      receivingData = true;
    } 
    else if (receivingData == true && receivedCharacter != endCharacter) {
      receivedCharacters[receivedCharcterIndex] = receivedCharacter;
      receivedCharcterIndex++;
      if (receivedCharcterIndex >= maxCharacters) {
        receivedCharcterIndex = maxCharacters - 1;
      }
    } 
    else if (receivingData == true && receivedCharacter == endCharacter) {
      receivedCharacters[receivedCharcterIndex] = '\0';
      receivedCharcterIndex = 0;
      receivingData = false;
      dataReady = true;
    }
  }
  if (dataReady == true) {
    dataReady = false;
    myservo.write(atoi(receivedCharacters));
    Serial.print("Value received: ");
    Serial.println(atoi(receivedCharacters));
  }
}