#include <esp_now.h>
#include <WiFi.h>

typedef struct struct_message {
  char letter;
} struct_message;

struct_message myData;

uint8_t broadcastAddress[] = {0x8C, 0x4F, 0x00, 0x29, 0x57, 0xE8};

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Trạng thái gửi '");
  Serial.print(myData.letter);
  Serial.print("': ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Thành công" : "Thất bại");
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Lỗi khởi tạo ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Thêm peer thất bại");
    return;
  }
}

void loop() {
  for (char c = 'a'; c <= 'z'; c++) {
    myData.letter = c;

    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

    if (result == ESP_OK) {
      Serial.print("Đang gửi ký tự: ");
      Serial.println(c);
    } else {
      Serial.println("Lỗi gửi dữ liệu");
    }

    delay(1000);
  }

  delay(3000);
}