// This code is a compilation of my own and others work, unfortunately I can't reference the authors I used
// as I have forgotten the sites I have taken from. But feel free to use and abuse this as much as you like.
// I hope it's useful and have fun,
// James. 

// So this is the code that we're going to be using. It looks long but mostly it's just me commenting on what 
// the code does. If you're not using a BMP 180, just remove the associated code. 

#include <SoftwareSerial.h> // Up to this point we've been using the RX TX pins(0 and 1) on the Arduino to communicate 
// with the ESP8266 by entering commands in the serial monitor command line. The SoftwareSerial library allows       
//those commands to be sent, and responses received via code and the digital input/output pins. 

#include <Wire.h> // Used to communicate with the BMP 180 via I2C.
#include <Adafruit_BMP085.h> // For BMP 180.
Adafruit_BMP085 bmp; // For BMP 180.

int ledPin = 13; // LED 13 will be used to let the user know when the code has looped back again.

// ThingSpeak Write API key, this can be found in your channel under API Keys and you'll want the "Write API Key". 
String apiKey = "NXUH8I9GHTMLJZPY";

#define SSID "Enter your router SSID here" // Remove the text but keep the ""
#define PASS "Enter your router password here"// Same as above, keep the ""

// connect pin 11 to TX of the ESP8266 (Yellow wire if you've been following the videos.)
// connect pin 10 to RX of the ESP8266 (Blue wire if you've been following the videos.)
SoftwareSerial SoftSer(11, 10); // Everywhere from here on when you see SoftSer, it is being used as a command
// in the same way you'd use Serial.print() or Serial.begin() only over pins 11 and 10 instead of the USB port.

void setup() 
{
  // Setting pin 13, the LED indicator, to output.
  pinMode(ledPin, OUTPUT);

  if (!bmp.begin()) // For BMP 180.
  {
    Serial.println("BMP180 sensor not found");
    while (1) {}
  }
  
  // Begins serial communication at a baud of 9600.
  Serial.begin(9600);
  
  // Begins software serial communication at a baud of 9600.
  SoftSer.begin(9600); 

}

void loop() 
{
  // Connecting to your internet source------------*
  // For your first time using this script, uncomment the below section of script and enter your router details above (SSID and Password).
  // After you successfully upload data, this can be commented back out as the ESP8266 will remember the router details.  
  /*String join="AT+CWJAP=\""; // Join accesspoint
  join+=SSID;
  join+="\",\"";
  join+=PASS;
  join+="\"";
  SoftSer.println(join);
  delay(5000);*/
  //-----------------------------------------------*


  // blink LED on board 
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);


  float temp = bmp.readTemperature(); // If you're not using a BMP 180, remove bmp.readTemperature(); and enter the 
  // analog pin you're reading from e.g: "float temp = analogRead(pin3);"

  // convert float to string-------------*
  char buf[16]; // buf array can hold up to 16 characters. 
  String strTemp = dtostrf(temp, 4, 1, buf); // "buf" will be the actual name of the array and strTemp is being equalled to it as a string.
  
  // dtostrf(floatvar, StringLengthIncDecimalPoint, numVarsAfterDecimal, charbuf);

  // floatvar ---- float variable
  // StringLengthIncDecimalPoint ---- This is the length of the string that will be created
  // numVarsAfterDecimal ---- The number of digits after the decimal point to print
  // charbuf ---- the array to store the results
  //-------------------------------------*
  Serial.println(strTemp); // Prints the recorded temperature to the serial print.

  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  SoftSer.println(cmd);

  if (SoftSer.find("Error")) 
  {
    Serial.println("AT+CIPSTART error");
    return;
  }

  // Putting together the GET string, this command structure can be found on your
  // ThingSpeak channel in Data Import/Export, under "Update Channel Feed - GET". 
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr += "&field1=";
  getStr += String(strTemp);
  getStr += "\r\n\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  SoftSer.println(cmd);

  if (SoftSer.find(">")) 
  {
    SoftSer.print(getStr); // Send data.
  }
  else 
  {
    SoftSer.println("AT+CIPCLOSE");
    
    Serial.println("AT+CIPCLOSE"); // If this shows on the serial monitor the data did not send.
  }
  // The AT+RST command resets the ESP8266, I'm doing this because data is uploaded much more reliably. 
  SoftSer.println("AT+RST");
  // ThingSpeak needs on average, a 15 second delay between updates. Change this to whatever suits your application
  // e.g: delay(600000) is ten minutes.
  delay(15000);
}

