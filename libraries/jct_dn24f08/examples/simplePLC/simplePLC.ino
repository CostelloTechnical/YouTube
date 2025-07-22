#include <jct_dn24f08.h>

dn24f08 plc;

void setup() {
  plc.init();
  plc.initCommunication(Serial, 9600,  '<', '>');
  plc.setDisplayEngineType(ANALOG);
  plc.setDisplayAnalogPin(V1);
  plc.setAnalogCalibration(V1, 1.1029, 0.0459);
  plc.setAnalogEngineType(TIME_MS, 100);
}

void loop() {
    plc.engine();
    if(plc.getAnalog(V1) > 4.5){
        plc.setOutput(CH1, true);
    } 
    else {
        plc.setOutput(CH1, false);
    }

    if(plc.getInput(IN1) == true){
        plc.setOutput(CH2, true);
    } 
    else {
        plc.setOutput(CH2, false);
    }

    if(plc.getDataReady() == true){
        if(strcmp(plc.getReceivedCharacters(),"DO,3,1")==0){
            plc.setOutput(CH3, true);
        }
        if(strcmp(plc.getReceivedCharacters(),"DO,3,0")==0){
            plc.setOutput(CH3, false);
        }
    }

    if(plc.getKeyPressed(KEY1)==true){
        plc.setOutput(CH4, !plc.getOutput(CH4));
    }
}