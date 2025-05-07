#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>

const char* ssid = "Giang";
const char* password = "88888888";

WiFiClient client;

unsigned long myChannelNumber = 2951790;
const char* myWriteAPIKey = "0DJSBAO30BP5FWU0";
const char* myReadAPIKey = "RJLL5UBCHKN8IRR4";

#define DHTPIN 25
#define DHTTYPE DHT11
#define LED2_PIN 2

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(LED2_PIN, OUTPUT);

  Serial.println("Connecting");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");

  ThingSpeak.begin(client);

  dht.begin();
}

void loop() {
  float humidity = dht.readHumidity();

  ThingSpeak.setField(1, humidity);
  int responseCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  float readHumidity = ThingSpeak.readFloatField(myChannelNumber, 1, myReadAPIKey);
  if (readHumidity > 80) {
    digitalWrite(LED2_PIN, HIGH);
  } else {
    digitalWrite(LED2_PIN, LOW);
  }

  delay(5000);
}