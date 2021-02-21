/* This is the code that will be uploaded to the Arduino. In this example, the built-in LED light of the Arduino will be controlled. If there are external LEDs, then change the pins accordingly */

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}
void loop() {
  if (Serial.available()>0){

    //Read the incoming serial data into a char variable
    char c = Serial.read();

    //Turn on the LED if an "h" is received from the ESP32
    if (c == 'h'){
      Serial.println("LED is ON");
      digitalWrite(LED_BUILTIN, HIGH);
    }

    //Turn off the LED if an "l" is received from the ESP32
    else if (c == 'l'){
      Serial.println("LED is OFF");
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
  delay(50);
}
