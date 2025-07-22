#include "jct_dn24f08.h"

dn24f08* dn24f08::_objectPointer = nullptr;
volatile uint8_t dn24f08::_previousPortB = 0;
volatile uint8_t dn24f08::_previousPortD = 0;

dn24f08::dn24f08(){}

void dn24f08::init(){

    _objectPointer = this;

    pinMode(_inData, INPUT);
    pinMode(_inClock, OUTPUT);
    pinMode(_inLoad, OUTPUT);

    pinMode(_key1, INPUT_PULLUP);
    pinMode(_key2, INPUT_PULLUP);
    pinMode(_key3, INPUT_PULLUP);
    pinMode(_key4, INPUT_PULLUP);
  
    pinMode(_outData, OUTPUT);
    pinMode(_outEnable, OUTPUT);
    pinMode(_outLoad, OUTPUT);
    pinMode(_outClock, OUTPUT);

    pinMode(_rxTxPin, OUTPUT);

    pinMode(_analogInputPins[I1], INPUT);
    pinMode(_analogInputPins[I2], INPUT);
    pinMode(_analogInputPins[I3], INPUT);
    pinMode(_analogInputPins[I4], INPUT);

    pinMode(_analogInputPins[V1], INPUT);
    pinMode(_analogInputPins[V2], INPUT);
    pinMode(_analogInputPins[V3], INPUT);
    pinMode(_analogInputPins[V4], INPUT);

    PCICR |= B00000101; // Enables pin change interrupts on ports B and D. 
    PCMSK0 |= B00000001; // Enables pin change interrupt on port B, pin 0. Pin 5 on the nano.
    PCMSK2 |= B11100000; // Enables pin change interrupts on port D, pins 5, 6 and 7. Pins 8, 7, 6 on the nano.
    
    _previousPortB = PINB;
    _previousPortD = PIND;

    setOutputs(0);
    _update = true;
    displayClear();
}

// Initialize communications.
void dn24f08::initCommunication(HardwareSerial& serialPort, uint32_t baud,  char startCharacter, char endCharacter){
  _serialPort = &serialPort;
  _serialPort->begin(baud);
  digitalWrite(_rxTxPin, false);

  _startCharacter = startCharacter;
  _endCharacter = endCharacter;
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

void dn24f08::setAnalogEngineType(engineAverageType type, uint16_t value){
    _analogAverageType = type;
    _analogAverageValue = value;
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

void dn24f08::setCheckButton(uint8_t pin){
    if(pin < _buttons){
        _checkButtons[pin] = true;
        _checkCache_ms[pin] = millis();
    }
}

bool dn24f08::getKeyPressed(uint8_t key){
    if(key > 0 && key < _buttons + 1){
        bool pressed = _pressed_flags[key - 1];
        _pressed_flags[key - 1] = false;
        return pressed;
    }
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

void dn24f08::engineAnalogAverage(){
    if(_iterator < _analogPins){
        bool valueReached = false;
        if (_analogAverageType == TIME_MS) {
            if (millis() - _averageTime_ms[_iterator] > _analogAverageValue) {
                 valueReached = true;
                 _averageTime_ms[_iterator] = millis();
            }
        }
        else if (_analogAverageType == READINGS) {
            if (_averageCounter[_iterator] >= _analogAverageValue) {
                valueReached = true;
            }
        }
        if(valueReached == true){
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

void dn24f08::engineButtons(){
    for(uint8_t i =0; i < _buttons; i++){
        if(_checkButtons[i] == true && ((millis() - _checkCache_ms[i]) >=_debounce_ms[i])){
            _pressed_flags[i] = digitalRead(_keys[i]) == HIGH;
            _checkButtons[i] = false;
        }
    }
}

void dn24f08::engineCommunication(){
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

void dn24f08::engine(){
    engineAnalogAverage();
    engineDisplay();
    engineButtons();
    engineCommunication();
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

void dn24f08::printS(String toPrint){
    digitalWrite(_rxTxPin, true);
    delayMicroseconds(500);
    _serialPort->print(toPrint);
    _serialPort->flush();
    digitalWrite(_rxTxPin, false);
}

void dn24f08::print(const char *toPrint){
    digitalWrite(_rxTxPin, true);
    delayMicroseconds(500);
    _serialPort->print(toPrint);
    _serialPort->flush();
    digitalWrite(_rxTxPin, false);
}

void dn24f08::println(const char *toPrint){
    digitalWrite(_rxTxPin, true);
    delayMicroseconds(500);
    _serialPort->println(toPrint);
    _serialPort->flush();
    digitalWrite(_rxTxPin, false);
}

bool dn24f08::getDataReady(){
  if (_dataReady == true){
    _dataReady = false;
    return true;
  }//
  else{
    return false;
  }
}

char* dn24f08::getReceivedCharacters(){
  return _receivedCharacters;
}

void dn24f08::setShift(uint8_t number, uint8_t digit, bool useDecimal) {
    digitalWrite(_outLoad, false);
    shiftOut(_outData, _outClock, MSBFIRST, _outputValue);
    shiftOut(_outData, _outClock, LSBFIRST, _digitEnable[digit]);
    shiftOut(_outData, _outClock, LSBFIRST, _segmentCharacters[number] + (_decimalPoint * useDecimal));
    digitalWrite(_outLoad, true);
}

ISR(PCINT0_vect) { // Pins D8-D13
    uint8_t portB = PINB;
    uint8_t changed_bits = portB ^ dn24f08::_previousPortB;
    dn24f08::_previousPortB = portB;
    if (!dn24f08::_objectPointer) return;
    if (changed_bits & 1 && (portB & 1) == 0) {
        dn24f08::_objectPointer->setCheckButton(3);
    }
}

ISR(PCINT2_vect) { // Pins D0-D7
    uint8_t portD = PIND;
    uint8_t changed_bits = portD ^ dn24f08::_previousPortD;
    dn24f08::_previousPortD = portD;
    if (!dn24f08::_objectPointer) return;

    for (uint8_t i = 5; i <= 7; i++) {
        if ((changed_bits >> i) & 1 && ((portD >> i) & 1) == 0) {
            dn24f08::_objectPointer->setCheckButton(i - 5);
        }
    }
}