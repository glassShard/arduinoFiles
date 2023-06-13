#include <WiFi.h>

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_MODE_STA);

  Serial.print("MAC address: ");
  Serial.print(WiFi.macAddress());

}

void loop() {
  // put your main code here, to run repeatedly:

}
