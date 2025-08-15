/*
  ==============================================================================
                                  DISCLAIMER
  ==============================================================================

  This software is provided "as is", without warranty of any kind, express or
  implied, including but not to the warranties of merchantability,
  fitness for a particular purpose and noninfringement. In no event shall the
  authors or copyright holders be liable for any claim, damages or other
  liability, whether in an action of contract, tort or otherwise, arising from,
  out of or in connection with the software or the use or other dealings in the
  software.

  ==============================================================================
                              PERMISSION TO USE
  ==============================================================================

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so.

  It is highly encouraged that if you find this library useful, you provide
  attribution back to the original author.
*/
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

    if (_receivingData == true && _receivedCharacter != _endCharacter) {
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
    else if (_receivedCharacter == _startCharacter) {
      _receivingData = true;
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