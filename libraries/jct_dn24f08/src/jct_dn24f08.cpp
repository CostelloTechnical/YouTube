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
#include "jct_dn24f08.h"

dn24f08* dn24f08::_classPointer = nullptr;
volatile uint8_t dn24f08::_previousPortB = 0;
volatile uint8_t dn24f08::_previousPortD = 0;

dn24f08::dn24f08(){}

// Initializer if not intending to use the Serial class.
void dn24f08::init(){
    // Used by the button ISR to point back to this class.
    _classPointer = this;

    // Setting the pin modes for the 8 inputs are on a 74HC165.
    pinMode(_inData, INPUT);
    pinMode(_inClock, OUTPUT);
    pinMode(_inLoad, OUTPUT);

    // Setting the pin modes for the 4 on-board buttons.
    pinMode(_key1, INPUT_PULLUP);
    pinMode(_key2, INPUT_PULLUP);
    pinMode(_key3, INPUT_PULLUP);
    pinMode(_key4, INPUT_PULLUP);
  
    // Setting the pin modes for the 8 outputs and 7 segment display controlled with 3 74HC595D
    pinMode(_outData, OUTPUT);
    pinMode(_outEnable, OUTPUT);
    pinMode(_outLoad, OUTPUT);
    pinMode(_outClock, OUTPUT);

    // Setting the pin mode for the RS485 IC Tx/Rx selector.
    pinMode(_rxTxPin, OUTPUT);

    // Setting the pin modes for the 4 current inputs.
    pinMode(_analogInputPins[I1], INPUT);
    pinMode(_analogInputPins[I2], INPUT);
    pinMode(_analogInputPins[I3], INPUT);
    pinMode(_analogInputPins[I4], INPUT);

    // Setting the pin modes for the 4 voltage inputs.
    pinMode(_analogInputPins[V1], INPUT);
    pinMode(_analogInputPins[V2], INPUT);
    pinMode(_analogInputPins[V3], INPUT);
    pinMode(_analogInputPins[V4], INPUT);

    PCICR |= B00000101; // Enables pin change interrupts on ports B and D. 
    PCMSK0 |= B00000001; // Enables pin change interrupt on port B, pin 0. Pin 8 on the nano.
    PCMSK2 |= B11100000; // Enables pin change interrupts on port D. These are pins 5, 6, 7 on the nano.
    
    _previousPortB = PINB; // Initialize the port b reading.
    _previousPortD = PIND; // Initialize the port b reading.

    setOutputs(0); // This sets the value of the digital outputs to zero. It's updated in display clear.
    _update = true; // Forces an update in display clear.
    displayClear(); // Clears the display and updates the digital outputs.
}

// Initializer if intending to use the Serial class with a start character and end character.
void dn24f08::init(HardwareSerial& serialPort, uint32_t baud,  char startCharacter, char endCharacter, uint16_t timeout){
    init();
    _serialPort = &serialPort;
    _serialPort->begin(baud);
    digitalWrite(_rxTxPin, false);

    _timeout = timeout;
    _useStartCharacter = true;
    _startCharacter = startCharacter;
    _endCharacter = endCharacter;
}

// Initializer if intending to use the Serial class with only an end character.
void dn24f08::init(HardwareSerial& serialPort, uint32_t baud, char endCharacter, uint16_t timeout){
    init();
    _serialPort = &serialPort;
    _serialPort->begin(baud);
    digitalWrite(_rxTxPin, false);

    _timeout = timeout;
    _useStartCharacter = false;
    _endCharacter = endCharacter;
    }

// Sets the value of the 8 outputs in binary. (Updated with display engine)
void dn24f08::setOutputs(uint8_t outputs){
    _outputValue = outputs;
}

// Set the value of a single output. (Updated with display engine)
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

// Set the offset and gain of an analog input.
void dn24f08::setAnalogCalibration(analogInputs input, float gain, float offset){
    _gains[input] = gain;
    _offsets[input] = offset;
}

/*  Set the type of analog engine to be used, a time or readings based system.
    The value is either milliseconds or number oif readings.*/
void dn24f08::setAnalogEngineType(engineAverageType type, uint16_t value){
    _analogAverageType = type;
    _analogAverageValue = value;
}

// Set if the display should be cleared, show an analog input or an integer.
void dn24f08::setDisplayEngineType(engineDisplayType type){
    _displayType = type;
    _update = true;
}

// Set the analog pin value to display.
void dn24f08::setDisplayAnalogPin(analogInputs pin){
    _displayAnalogPin = pin;
}

// Set the integer value to display.
void dn24f08::setDisplayInteger(uint16_t number){
    _displayNumber = number;
}

// If a pin change was detected this is called to cache the time for debouncing.
void dn24f08::setCheckButton(uint8_t pin){
    if(pin < _buttons){
        _checkButtons[pin] = true;
        _checkCache_ms[pin] = millis();
    }
}

// Returns if a button press was registered.
bool dn24f08::getKeyPressed(uint8_t key){
    if(key > 0 && key < _buttons + 1){
        bool pressed = _pressed_flags[key - 1];
        _pressed_flags[key - 1] = false;
        return pressed;
    }
}

// Returns the 8 output values as a binary number.
uint8_t dn24f08::getOutputs(){
    return _outputValue;
}

// Returns the state of an output.
bool dn24f08::getOutput(uint8_t output){
    if(output <= 8 && output > 0){
        output --;
        return _outputValue & (1 << output);
    }
}

// Returns the 8 input values as a binary number.
uint8_t dn24f08::getInputs(){
    _inputValue = 0;
    digitalWrite(_inLoad, LOW);
    digitalWrite(_inLoad, HIGH);
    for (uint8_t i = 0; i < 8; ++i) {
        if (i > 0) {
            digitalWrite(_inClock, HIGH);
        }
        _inputValue |= digitalRead(_inData) << (7 - i);
        digitalWrite(_inClock, LOW);
    }
    return _inputValue;
}

// Returns the state of an input.
bool dn24f08::getInput(uint8_t input){
    getInputs();
    return (_inputValue & (1 << input)) == 0;
}

// Returns the value of an analog input.
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

// Returns the averaged value of an analog input. (analog engine must be running)
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

// Handles the averaging of the analog inputs as per the type. (Time or readings)
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

// Handles the different display types and updating the outputs.
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
            
        default:
            displayClear();
            break;
    }
}

// Handles the buttons. Checks if a buttons was pressed, including debounce.
void dn24f08::engineButtons(){
    for(uint8_t i =0; i < _buttons; i++){
        if(_checkButtons[i] == true && ((millis() - _checkCache_ms[i]) >=_debounce_ms[i])){
            _pressed_flags[i] = digitalRead(_keys[i]) == HIGH;
            _checkButtons[i] = false;
        }
    }
}

// Handles incoming serial data.
void dn24f08::engineCommunication(){
    if(millis() - _timeoutCache > _timeout && _dataReady == false && _receivingData == true){
        _receivedCharacters[_receivedCharacterIndex] = '\0';
        _receivedCharacterIndex = 0;
        _receivingData = false;
        _timedOut = true;
    }
    else if (_serialPort->available() > 0) {
        char _receivedCharacter = _serialPort->read();
        if(_useStartCharacter == false && _receivingData == false){
            _receivingData = true;
            _timeoutCache = millis();
        }

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
            _timedOut = false;
            _dataReady = true;
        }
        else if (_receivedCharacter == _startCharacter && _useStartCharacter == true) {
            _receivingData = true;
            _timeoutCache = millis();
        }
    }
}

// Wrapper for the other engines.
void dn24f08::engine(){
    engineAnalogAverage();
    engineDisplay();
    engineButtons();
    engineCommunication();
}

// Displays the float on the 7 segment display.
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

// Displays the integer on the 7 segment display.
void dn24f08::displayInteger(uint16_t number) {
    sprintf(_converter, "%d", number);
    uint8_t offset = strlen(_converter);
    for (uint8_t i = 0; i < 5; i++) {
      setShift(_converter[i] - '0', i + 4 - offset, false);
    }
}

// Clears the 7 segment display.
void dn24f08::displayClear() {
    if(_update){
        for (uint8_t i = 0; i < 5; i++) {
            setShift(36, i, false);
        }
        _update = false;
    }
}

// Print a String over RS485
void dn24f08::printS(String toPrint){
    digitalWrite(_rxTxPin, true);
    delayMicroseconds(500);
    _serialPort->print(toPrint);
    _serialPort->flush();
    digitalWrite(_rxTxPin, false);
}

// Print a c-string over RS485
void dn24f08::print(const char *toPrint){
    digitalWrite(_rxTxPin, true);
    delayMicroseconds(500);
    _serialPort->print(toPrint);
    _serialPort->flush();
    digitalWrite(_rxTxPin, false);
}

// Print a c-string over RS485 with a newline
void dn24f08::println(const char *toPrint){
    digitalWrite(_rxTxPin, true);
    delayMicroseconds(500);
    _serialPort->println(toPrint);
    _serialPort->flush();
    digitalWrite(_rxTxPin, false);
}

// Returns true if the communication engine received a valid message.
bool dn24f08::getDataReady(){
  if (_dataReady == true){
    _dataReady = false;
    return true;
  }//
  else{
    return false;
  }
}

// Returns if there was a timeout.
bool dn24f08::getTimedOut(){
  if (_timedOut == true){
    _timedOut = false;
    return true;
  }//
  else{
    return false;
  }
}

// Returns the received message.
char* dn24f08::getReceivedCharacters(){
  return _receivedCharacters;
}

// Writes to the three 74HC595D ICs controlling the digital outputs and 7 segment display.
void dn24f08::setShift(uint8_t number, uint8_t digit, bool useDecimal) {
    digitalWrite(_outLoad, false);
    shiftOut(_outData, _outClock, MSBFIRST, _outputValue);
    shiftOut(_outData, _outClock, LSBFIRST, _digitEnable[digit]);
    shiftOut(_outData, _outClock, LSBFIRST, _segmentCharacters[number] + (_decimalPoint * useDecimal));
    digitalWrite(_outLoad, true);
}

// Handles the pin change interrupt for the button on pin 8.
ISR(PCINT0_vect) { // Pins D8-D13
    uint8_t portB = PINB;
    uint8_t changed_bits = portB ^ dn24f08::_previousPortB;
    dn24f08::_previousPortB = portB;
    if (!dn24f08::_classPointer) return;
    if (changed_bits & 1 && (portB & 1) == 0) {
        dn24f08::_classPointer->setCheckButton(3);
    }
}

// Handles the pin change interrupt for the buttons on pin 5, 6 and 7.
ISR(PCINT2_vect) { // Pins D0-D7
    uint8_t portD = PIND;
    uint8_t changed_bits = portD ^ dn24f08::_previousPortD;
    dn24f08::_previousPortD = portD;
    if (!dn24f08::_classPointer) return;

    for (uint8_t i = 5; i <= 7; i++) {
        if ((changed_bits >> i) & 1 && ((portD >> i) & 1) == 0) {
            dn24f08::_classPointer->setCheckButton(i - 5);
        }
    }
}