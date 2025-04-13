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