#include "dn24f08.h"

dn24f08::dn24f08(){}

void dn24f08::init(){
    pinMode(_inData, OUTPUT);
    pinMode(_inClock, OUTPUT);
    pinMode(_inLoad, OUTPUT);
  
    pinMode(_outData, OUTPUT);
    pinMode(_outEnable, OUTPUT);
    pinMode(_outLoad, OUTPUT);
    pinMode(_outClock, OUTPUT);

    pinMode(_analogInputPins[I1], INPUT);
    pinMode(_analogInputPins[I2], INPUT);
    pinMode(_analogInputPins[I3], INPUT);
    pinMode(_analogInputPins[I4], INPUT);

    pinMode(_analogInputPins[V1], INPUT);
    pinMode(_analogInputPins[V2], INPUT);
    pinMode(_analogInputPins[V3], INPUT);
    pinMode(_analogInputPins[V4], INPUT);
}

void dn24f08::setOutputs(uint8_t outputs){
    _outputValue = outputs;
}

void dn24f08::setOutput(uint8_t output, bool state){
    if(output <= 8 && output > 0){
        if(state){
            _outputValue += 1 << (output - 1);
        }
        else{
            _outputValue -= 1 << (output - 1);
        }
    }
}

void dn24f08::setAnalogCalibration(analogInputs input, float gain, float offset){
    _gains[input] = gain;
    _offsets[input] = offset;
}

void dn24f08::setShift(uint8_t number, uint8_t digit, bool useDecimal) {
    digitalWrite(_outLoad, false);
    shiftOut(_outData, _outClock, MSBFIRST, _outputValue);
    shiftOut(_outData, _outClock, LSBFIRST, _digitEnable[digit]);
    shiftOut(_outData, _outClock, LSBFIRST, _segmentNumbers[number] + (_decimalPoint * useDecimal));
    digitalWrite(_outLoad, true);
}

float dn24f08::getAnalog(analogInputs input){
    if(input >= I1 && input <= I4){
        // Returns  milliamps for I1-I4.
        return (analogRead(_analogInputPins[input]) * 20.0 / 1023.0) * _gains[input] + _offsets[input];
    }
    else if( input >= V1 && input <= V4 ){
        // Returns a voltage for V1-V4.
        return (analogRead(_analogInputPins[input]) * 10.0 / 1023.0) * _gains[input] + _offsets[input];
    }
}

float dn24f08::getAnalogAverage(analogInputs input){
    if(input >= I1 && input <= I4){
        // Returns average milliamps for I1-I4.
        return (_averageAnalog[input] * 20.0 / 1023.0) * _gains[input] + _offsets[input];
    }
    else if( input >= V1 && input <= V4 ){
        // Returns average voltage for V1-V4.
        return (_averageAnalog[input] * 10.0 / 1023.0) * _gains[input] + _offsets[input];
    }
}

void dn24f08::analogAverageTime(uint16_t duration_ms){
    if(_iterator < _analogPins){
        if (millis() - _averageTime_ms[_iterator] > duration_ms) {
            _averageAnalog[_iterator] = (float)_averageSum[_iterator] / _averageCounter[_iterator];
            _averageSum[_iterator] = 0;
            _averageCounter[_iterator] = 0;
            _averageTime_ms[_iterator] = millis();
        }
        else {
            _averageSum[_iterator] += analogRead(_analogInputPins[_iterator]);
            _averageCounter[_iterator]++;
        }
        _iterator++;
    }
    else{
        _iterator = 0;
    }
}

void dn24f08::analogAverageReadings(uint16_t readings){
    if(_iterator < _analogPins){
        if (_averageCounter[_iterator] >= readings) {
            _averageAnalog[_iterator] = (float)_averageSum[_iterator] / _averageCounter[_iterator];
            _averageSum[_iterator] = 0;
            _averageCounter[_iterator] = 0;
        }
        else {
            _averageSum[_iterator] += analogRead(_analogInputPins[_iterator]);
            _averageCounter[_iterator]++;
        }
        _iterator++;
    }
    else{
        _iterator = 0;
    }
}

void dn24f08::displayFloat(float number) {
    char* converter;
    dtostrf(number, 0, 3, converter);
    uint8_t decimalIndex = (strchr(converter, '.') - converter);
    uint8_t decimalOffset = 0;
    for (uint8_t i = 0; i < 5; i++) {
      if (i == decimalIndex) {
        decimalOffset = 1;
      }  //
      else {
        setShift(converter[i] - '0', i - decimalOffset, i == decimalIndex - 1);
      }
    }
}

void dn24f08::displayInteger(uint16_t number) {
    char* converter;
    sprintf(converter, "%d", number);
    uint8_t offset = strlen(converter);
    for (uint8_t i = 0; i < 5; i++) {
      setShift(converter[i] - '0', i + 4 - offset, false);
    }
}