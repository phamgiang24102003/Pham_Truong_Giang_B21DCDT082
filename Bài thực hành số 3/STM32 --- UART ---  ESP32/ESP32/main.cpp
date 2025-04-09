#include <SoftwareSerial.h>

SoftwareSerial mySerial(4, 5);

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop() {

  if(Serial.available() > 0){
    String input = Serial.readString();
    mySerial.println(input);    
  }
 
  if(mySerial.available() > 1){
    String input = mySerial.readString();
    Serial.println(input);    
  }
  delay(20);
}