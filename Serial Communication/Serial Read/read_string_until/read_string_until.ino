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
    if (strcmp(receivedCharacters, "led_on") == 0) {
      Serial.println(receivedCharacters);
      digitalWrite(LED_BUILTIN, HIGH);

    } else if (strcmp(receivedCharacters, "led_off") == 0) {
      Serial.println(receivedCharacters);
      digitalWrite(LED_BUILTIN, LOW);

    } else {
      Serial.print("Unknown command: ");
      Serial.println(receivedCharacters);
    }
  }
}