#include <jct_dn24f08.h>

dn24f08 plc;

void setup() {
  plc.init(Serial, 9600,  '<', '>');
  plc.setDisplayEngineType(ANALOG);
  plc.setDisplayAnalogPin(V1);
  plc.setAnalogCalibration(V1, 1.1029, 0.015);
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

    if(plc.getKeyPressed(KEY1)==true){
        plc.setOutput(CH3, !plc.getOutput(CH3));
    }

    if(plc.getDataReady() == true){
        if(strcmp(plc.getReceivedCharacters(),"CH4_ON")==0){
            plc.setOutput(CH4, true);
        }
        if(strcmp(plc.getReceivedCharacters(),"CH4_OFF")==0){
            plc.setOutput(CH4, false);
        }
    }
}