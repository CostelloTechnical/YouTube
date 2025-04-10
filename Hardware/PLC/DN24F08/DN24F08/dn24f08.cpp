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

    pinMode(_analogPins[I1], INPUT);
    pinMode(_analogPins[I2], INPUT);
    pinMode(_analogPins[I3], INPUT);
    pinMode(_analogPins[I4], INPUT);

    pinMode(_analogPins[V1], INPUT);
    pinMode(_analogPins[V2], INPUT);
    pinMode(_analogPins[V3], INPUT);
    pinMode(_analogPins[V4], INPUT);
}

void dn24f08::setOutputs(uint8_t outputs){
    _outputs = outputs;
}

void dn24f08::setOutput(uint8_t output, bool state){
    if(output <= 8 && output > 0){
        if(state){
            _outputs += 1 << (output - 1);
        }
        else{
            _outputs -= 1 << (output - 1);
        }
    }
}

void dn24f08::setAnalogCalibration(analogInputs input, float gain, float offset){
    _gains[input] = gain;
    _offsets[input] = offset;
}

void dn24f08::setShift(uint8_t number, uint8_t digit, bool useDecimal) {
    digitalWrite(_outLoad, false);
    shiftOut(_outData, _outClock, MSBFIRST, _outputs);
    shiftOut(_outData, _outClock, LSBFIRST, _digitEnable[digit]);
    shiftOut(_outData, _outClock, LSBFIRST, _segmentNumbers[number] + (_decimalPoint * useDecimal));
    digitalWrite(_outLoad, true);
}

float dn24f08::getAnalog(analogInputs input){
    return (analogRead(_analogPins[input])* 10.0 / 1023.0) * _gains[input] + _offsets[input];
}

float dn24f08::getAnalogAverage(analogInputs input){
    return _averageAnalog[input];
}

void dn24f08::analogAverageTime(uint16_t duration_ms){
    if(_iterator < 8){
        if (millis() - _averageTime_ms[_iterator] > duration_ms) {
            _averageAnalog[_iterator] = ((_averageSum[_iterator] / _averageCounter[_iterator]) * 10.0 / 1023.0)  * _gains[_iterator] + _offsets[_iterator];
            _averageSum[_iterator] = 0;
            _averageCounter[_iterator] = 0;
            _averageTime_ms[_iterator] = millis();
        }
        else {
            _averageSum[_iterator] += analogRead(_analogPins[_iterator]);
            _averageCounter[_iterator]++;
        }
        _iterator++;
    }
    else{
        _iterator = 0;
    }
}

void dn24f08::analogAverageReadings(uint16_t readings){
    if(_iterator < 8){
        if (_averageCounter[_iterator] >= readings) {
            _averageAnalog[_iterator] = ((_averageSum[_iterator] / _averageCounter[_iterator]) * 10.0 / 1023.0)  * _gains[_iterator] + _offsets[_iterator];
            _averageSum[_iterator] = 0;
            _averageCounter[_iterator] = 0;
        }
        else {
            _averageSum[_iterator] += analogRead(_analogPins[_iterator]);
            _averageCounter[_iterator]++;
        }
        _iterator++;
    }
    else{
        _iterator = 0;
    }
}

void dn24f08::displayFloat(float number) {
    dtostrf(number, 0, 3, _converter);
    uint8_t decimalIndex = (strchr(_converter, '.') - _converter);
    uint8_t decimalOffset = 0;
    for (uint8_t i = 0; i < 5; i++) {
      if (i == decimalIndex) {
        decimalOffset = 1;
      }  //
      else {
        setShift(_converter[i] - '0', i - decimalOffset, i == decimalIndex - 1);
      }
    }
}

void dn24f08::displayInteger(uint16_t number) {
    uint8_t offset;
    sprintf(_converter, "%d", number);
    offset = strlen(_converter);
    for (uint8_t i = 0; i < 5; i++) {
      setShift(_converter[i] - '0', i + 4 - offset, false);
    }
}