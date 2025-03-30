float pi = 3.1415;
char buffer[20];
char toPrint[20];

void setup() {
  Serial.begin(9600);
  while(!Serial){}
  dtostrf(pi, 0, 2, buffer);
  sprintf(toPrint, "<%s>", buffer);
  Serial.println(toPrint);
}

void loop() {
}