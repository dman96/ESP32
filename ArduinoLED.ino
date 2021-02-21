
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}
void loop() {
  if (Serial.available()>0){

    uint8_t c = "";
    c = Serial.read();

    if (c == 'h'){
      Serial.println("LED is ON");
      digitalWrite(LED_BUILTIN, HIGH);
    }

    else if (c == 'l'){
      Serial.println("LED is OFF");
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
  delay(50);
}
