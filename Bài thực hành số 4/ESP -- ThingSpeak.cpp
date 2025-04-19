#include <WiFi.h>
#include <ThingSpeak.h>

//https://api.thingspeak.com/update?api_key=0QQ6WIA3FQ5EYUPK&field1=1
//https://api.thingspeak.com/update?api_key=0QQ6WIA3FQ5EYUPK&field1=0

const char* ssid = "Giang";
const char* password = "88888888";

WiFiClient client;

unsigned long myChannelNumber = 2920255;
const char* myReadAPIKey = "HEVBUQZM0NWPHD1E";
const char* myWriteAPIKey = "0QQ6WIA3FQ5EYUPK";

const int ledPin = 2;
const int switchPin = 5;

void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(switchPin, INPUT);

  Serial.println("Đang kết nối WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi đã kết nối!");

  ThingSpeak.begin(client);
}

void loop() {
  int led_status = ThingSpeak.readFloatField(myChannelNumber, 1, myReadAPIKey);
  int switchState = digitalRead(switchPin);
  
  if (switchState == HIGH) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  ThingSpeak.setField(1, switchState);
  int responseCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  delay(100);
}