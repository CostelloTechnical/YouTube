#include <jct_dn24f08.h>

dn24f08 plc;

void setup() {
  plc.init();
  plc.initCommunication(Serial, 9600,  '<', '>');
}

void loop() {
    plc.engineButtons();
    if(plc.getKeyPressed(KEY1)==true){
        plc.println("Key1 Pressed");
    }
    if(plc.getKeyPressed(KEY2)==true){
        plc.println("Key2 Pressed");
    }
    if(plc.getKeyPressed(KEY3)==true){
        plc.println("Key3 Pressed");
    }
    if(plc.getKeyPressed(KEY4)==true){
        plc.println("Key4 Pressed");
    }
}