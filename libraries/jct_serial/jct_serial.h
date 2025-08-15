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
#ifndef JCT_SERIAL_H
#define JCT_SERIAL_H
#include <Arduino.h>

class jctSerial
{
public:
    jctSerial();
    void init(HardwareSerial& serialPort, uint32_t baud);
    void init(HardwareSerial& serialPort, uint32_t baud, uint8_t rxTxPin);
    void setTerminationCharacters(char start, char end);
    void check();
    void printS(String toPrint);
    void print(const char *toPrint);
    void println(const char *toPrint);
    bool getDataReady();
    char* getReceivedCharacters();

private:
    bool _dataReady = false;
    bool _receivingData  = false;
    const uint8_t _maxCharacters = 255;
    uint8_t _element = 0;
    uint8_t _rxTxPin;
    bool _useRxTxPin = false;
    char _startCharacter = '<';
    char _endCharacter = '>';
    char _receivedCharacter;
    uint8_t _receivedCharacterIndex = 0;
    char _receivedCharacters[255];
    HardwareSerial* _serialPort;
};
#endif