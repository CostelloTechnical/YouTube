const char startCharacter = '<';
const char endCharacter ='>';
const uint8_t maxCharacters = 50;
char receivedCharacters[maxCharacters];
bool receivingData = false;
uint8_t receivedCharcterIndex = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  if(Serial.available() > 0){
    char receivedCharacter = Serial.read();

    if(receivedCharacter == startCharacter){
      receivingData = true;
    }
    else if(receivingData == true && receivedCharacter != endCharacter){
      receivedCharacters[receivedCharcterIndex] = receivedCharacter;
      receivedCharcterIndex ++;
    }
    else if(receivingData == true && receivedCharacter == endCharacter){
      receivedCharacters[receivedCharcterIndex] = '\0';
      for(uint8_t i = 0; i < receivedCharcterIndex; i++){
        Serial.write(receivedCharacters[i]);
      }
      receivedCharcterIndex = 0;
      receivingData = false;
      Serial.println();
    }
  }
}