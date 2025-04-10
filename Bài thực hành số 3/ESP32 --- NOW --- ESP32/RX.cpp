#include <esp_now.h>
#include <WiFi.h>

typedef struct struct_message {
  char letters;
} struct_message;

struct_message receivedData;

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  Serial.print("Ky tu nhan duoc: ");
  Serial.println(receivedData.letters);
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  delay(2000);
}