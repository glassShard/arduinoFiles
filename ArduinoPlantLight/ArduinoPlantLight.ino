#include <DS3231.h>
#include <Wire.h>
#include <time.h>

DS3231 rtc;
uint8_t LIGHTPIN = A0;
uint8_t RELAYPIN = 7;
uint8_t relayState = LOW;

String setLeadingZero(int value) {
  if (value < 10) {
    return 0 + String(value);
  }
  return String(value);
}

void setRelay(uint8_t value) {
  digitalWrite(RELAYPIN, value);
  relayState = value;
}

void setup() {
	Serial.begin(9600);
	Wire.begin();

  pinMode(RELAYPIN, OUTPUT);
  pinMode(LIGHTPIN, INPUT);

  const DateTime dnow = RTClib::now();
  Serial.print(dnow.year());
  Serial.print("-");
  Serial.print(setLeadingZero(dnow.month()));
  Serial.print("-");
  Serial.print(setLeadingZero(dnow.day()));
  Serial.print(" ");
  Serial.print(setLeadingZero(dnow.hour()));
  Serial.print(":");
  Serial.print(setLeadingZero(dnow.minute()));
  Serial.print(":");
  Serial.println(setLeadingZero(dnow.second()));
}

void loop() {
  const int light = analogRead(LIGHTPIN);
  Serial.println(light);
  const DateTime d = RTClib::now();
  
  if (relayState == HIGH) {
    if (d.hour() < 6 || d.hour() > 20 || light > 810) {
      setRelay(LOW);
    } 
  }

  if (relayState == LOW) {
    if (d.hour() > 6 && d.hour() < 20 && light < 750) {
      setRelay(HIGH);
    } 
  }

  delay(10000);
}
