#include "jct_serial.h"

jctSerial::jctSerial(){}

void jctSerial::init(HardwareSerial& serialPort, uint32_t baud){
  _serialPort = &serialPort;
  _serialPort->begin(baud);
}

void jctSerial::init(HardwareSerial& serialPort, uint32_t baud, uint8_t rxTxPin){
  _serialPort = &serialPort;
  _serialPort->begin(baud);
  _rxTxPin = rxTxPin;
  _useRxTxPin = true;
  pinMode(_rxTxPin, OUTPUT);
  digitalWrite(_rxTxPin, false);
}

void jctSerial::setTerminationCharacters(char start, char end){
  _startCharacter = start;
  _endCharacter = end;
}

void jctSerial::check(){
  if (_serialPort->available() > 0) {
    char _receivedCharacter = _serialPort->read();
    Serial.println(_receivedCharacter, HEX);

    if (_receivedCharacter == _startCharacter) {
      _receivingData = true;
    } 
    else if (_receivingData == true && _receivedCharacter != _endCharacter) {
      _receivedCharacters[_receivedCharacterIndex] = _receivedCharacter;
      _receivedCharacterIndex++;
      if (_receivedCharacterIndex >= _maxCharacters) {
        _receivedCharacterIndex = _maxCharacters - 1;
      }
    } 
    else if (_receivingData == true && _receivedCharacter == _endCharacter) {
      _receivedCharacters[_receivedCharacterIndex] = '\0';
      _receivedCharacterIndex = 0;
      _receivingData = false;
      _dataReady = true;
    }
  }
}

void jctSerial::printS(String toPrint){
  if(_useRxTxPin){
    digitalWrite(_rxTxPin, true);
    _serialPort->print(toPrint);
    _serialPort->flush();
    digitalWrite(_rxTxPin, false);
  }
  else{
    _serialPort->print(toPrint);
  }
}

void jctSerial::print(const char *toPrint){
  if(_useRxTxPin){
    digitalWrite(_rxTxPin, true);
    _serialPort->print(toPrint);
    _serialPort->flush();
    digitalWrite(_rxTxPin, false);
  }
  else{
    _serialPort->print(toPrint);
  }
}

void jctSerial::println(const char *toPrint){
  if(_useRxTxPin){
    digitalWrite(_rxTxPin, true);
    _serialPort->println(toPrint);
    _serialPort->flush();
    digitalWrite(_rxTxPin, false);
  }
  else{
    _serialPort->println(toPrint);
  }
}

bool jctSerial::getDataReady(){
  if (_dataReady == true){
    _dataReady = false;
    return true;
  }//
  else{
    return false;
  }
}

char* jctSerial::getReceivedCharacters(){
  return _receivedCharacters;
}