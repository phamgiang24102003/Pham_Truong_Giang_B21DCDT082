#include <SoftwareSerial.h>

SoftwareSerial mySerial(4, 5);

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop() {
  if (mySerial.available()) {
    String input = mySerial.readStringUntil('\n');
    Serial.println(input);
  }
  delay(20);
}