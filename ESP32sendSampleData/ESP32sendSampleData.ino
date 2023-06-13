#include <esp_now.h>
#include <WiFi.h>

const int SensorPin = 32;
const int SensorNumber = 1;

uint8_t responderAddress[] = {0x0C, 0xB8, 0x15, 0x77, 0x0E, 0xD4};

typedef struct struct_message {
  int sensorNumber;
  int readValue;
} struct_message;

struct_message myData;

esp_now_peer_info_t peerInfo;

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP_NOW");

    return;
  }

  esp_now_register_send_cb(onDataSent);

  memcpy(peerInfo.peer_addr, responderAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");

    return ;
  }
}

void loop() {
  myData.sensorNumber = SensorNumber;
  myData.readValue = analogRead(SensorPin);

  esp_err_t result = esp_now_send(responderAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sending error");
  }

  delay(2000);
}
