#include <BH1750.h>
#include <Wire.h>

BH1750 lightMeter;

void setup() {
    Serial.begin(9600);
    Wire.begin();
    lightMeter.begin();
    Serial.println(F("BH1750 Test begin"));
}

void loop() {
    float lux = lightMeter.readLightLevel();
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.print(" lx\n");

    delay(1000);
}