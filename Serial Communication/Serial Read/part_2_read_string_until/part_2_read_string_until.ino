String command;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    command = Serial.readStringUntil('\n');
    if (command.equals("led_on")) {
      Serial.println("Turning on!");
      digitalWrite(LED_BUILTIN, HIGH);

    } 
    else if (command.equals("led_off")) {
      Serial.println("Turning off!");
      digitalWrite(LED_BUILTIN, LOW);
    } 
    else {
      Serial.print("Unknown command: ");
      Serial.println(command);
    }
  }
}