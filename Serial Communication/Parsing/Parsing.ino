const char startCharacter = '<';
const char endCharacter = '>';
const uint8_t maxCharacters = 10;
char receivedCharacters[maxCharacters];
char toParse[maxCharacters];
char action[4];
uint8_t pinNumber;
bool pinState;

bool receivingData = false;
bool dataReady = false;
uint8_t receivedCharcterIndex = 0;

void setup() {
  Serial.begin(9600);
  pinMode(12, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(3, OUTPUT);
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

    strcpy(toParse, receivedCharacters);
    strtok(toParse, ",");

    if (sizeof(action) > strlen(toParse)) {
      strcpy(action, toParse);
    }

    pinNumber = atoi(strtok(NULL, ","));

    if (strcmp(action, "set") == 0) {
      pinState = atoi(strtok(NULL, ","));
      digitalWrite(pinNumber,pinState);
      sprintf(toParse,"<%s,%d,%d>", action, pinNumber,digitalRead(pinNumber));
      Serial.println(toParse);
    }
    else if(strcmp(action, "get") == 0){
      sprintf(toParse,"<%s,%d,%d>", action, pinNumber, digitalRead(pinNumber));
      Serial.println(toParse);
    }
    else{
      Serial.print("Unknown command: ");
      Serial.println(receivedCharacters);
    }
  }
}