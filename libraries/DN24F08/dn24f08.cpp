#include "dn24f08.h"

dn24f08::dn24f08(){}

void dn24f08::init(){
    pinMode(_inData, INPUT);
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

    setOutputs(0);
    setDisplayEngineType(CLEAR);
}

void dn24f08::setOutputs(uint8_t outputs){
    _outputValue = outputs;
}

void dn24f08::setOutput(uint8_t output, bool state){
    if(output <= 8 && output > 0){
        _update = true;
        output --;
        if(state){
            _outputValue |= (1 << output);
        }
        else{
            _outputValue &= ~(1 << output);
        }
    }
}

void dn24f08::setAnalogCalibration(analogInputs input, float gain, float offset){
    _gains[input] = gain;
    _offsets[input] = offset;
}

void dn24f08::setAnalogEngineType(engineAverageType type){

}

void dn24f08::setDisplayEngineType(engineDisplayType type){
    _displayType = type;
    _update = true;
}

void dn24f08::setDisplayAnalogPin(analogInputs pin){
    _displayAnalogPin = pin;
}

void dn24f08::setDisplayInteger(uint16_t number){
    _displayNumber = number;
}

uint8_t dn24f08::getOutputs(){
    return _outputValue;
}

bool dn24f08::getOutput(uint8_t output){
    if(output <= 8 && output > 0){
        output --;
        return _outputValue & (1 << output);
    }
}

uint8_t dn24f08::getInputs(){
    digitalWrite(_inLoad, HIGH);
    delayMicroseconds(5);
    digitalWrite(_inClock, HIGH);
    _inputValue = shiftIn(_inData, _inClock, MSBFIRST);
    digitalWrite(_inLoad, LOW);
    return _inputValue;
}

bool dn24f08::getInput(uint8_t input){
    getInputs();
    return (_inputValue & (1 << input)) == 0;
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

void dn24f08::engineAnalogAverage_ms(uint16_t duration_ms){
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

void dn24f08::engineAnalogAverage_readings(uint16_t readings){
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

void dn24f08::engineDisplay(){
    switch (_displayType) {
        // Case for when currentState is RED
        case IDLE:
            displayClear();
            break;

        case CLEAR:
            displayClear();
            _displayType = IDLE;
            break;

        case ANALOG:
            displayFloat(getAnalogAverage(_displayAnalogPin));
            break;

        case INTEGER:
            displayInteger(_displayNumber);
            break;

        case CHARACTERS:
            
            break;
            
        default:
            displayClear();
            break;
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
    sprintf(_converter, "%d", number);
    uint8_t offset = strlen(_converter);
    for (uint8_t i = 0; i < 5; i++) {
      setShift(_converter[i] - '0', i + 4 - offset, false);
    }
}

void dn24f08::displayClear() {
    if(_update){
        for (uint8_t i = 0; i < 5; i++) {
            setShift(36, i, false);
        }
        _update = false;
    }
}

void dn24f08::setShift(uint8_t number, uint8_t digit, bool useDecimal) {
    digitalWrite(_outLoad, false);
    shiftOut(_outData, _outClock, MSBFIRST, _outputValue);
    shiftOut(_outData, _outClock, LSBFIRST, _digitEnable[digit]);
    shiftOut(_outData, _outClock, LSBFIRST, _segmentCharacters[number] + (_decimalPoint * useDecimal));
    digitalWrite(_outLoad, true);
}