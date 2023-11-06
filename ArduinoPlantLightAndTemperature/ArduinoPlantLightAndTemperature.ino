#include <DS3231.h>
#include <Wire.h>
#include <time.h>
#include "DHT.h"
#include <LiquidCrystal_I2C.h>

DS3231 rtc;
uint8_t LIGHTPIN = A0;
uint8_t DHTPIN = 2;
uint8_t RELAYLIGHTPIN = 7;
uint8_t RELAYHEATPIN = 6;
uint8_t relayLightState = LOW;
uint8_t relayHeatState = LOW;

#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

String setLeadingZero(int value) {
  if (value < 10) {
    return 0 + String(value);
  }
  return String(value);
}

void setRelay(uint8_t relayPin, uint8_t value) {
  // a heat relé fordítva működik, ezért heat esetben meg kell fordítani a relére kimenő jelet
  int valueToSet = value;
  Serial.println("SetRelay called");
  Serial.println(relayPin);
  
  if (relayPin == RELAYLIGHTPIN) {
    relayLightState = value;
  }

  if (relayPin == RELAYHEATPIN) {
    if (value == HIGH) {
      valueToSet = LOW;
    } else {
      valueToSet = HIGH;
    }
    relayHeatState = value;
    Serial.print("RELAYHEATPIN: ");
    Serial.println(value);
  }

  digitalWrite(relayPin, valueToSet);
}

void printOnLcd(float t, float h) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(t);
    lcd.print("C ");
    lcd.print(h); 
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print("Heat: ");
    if (relayHeatState == HIGH) {
      lcd.print("+ ");
    } else {
      lcd.print("- ");
    }
    lcd.print("Light: ");
    if (relayLightState == HIGH) {
      lcd.print("+");
    } else {
      lcd.print("-");
    }
}

void setup() {
	Serial.begin(9600);
	Wire.begin();

  pinMode(RELAYLIGHTPIN, OUTPUT);
  pinMode(RELAYHEATPIN, OUTPUT);
  pinMode(LIGHTPIN, INPUT);
  pinMode(DHTPIN, INPUT);

  dht.begin();

  lcd.init();
  //lcd.backlight();

  digitalWrite(RELAYHEATPIN, HIGH);

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
  
  if (relayLightState == HIGH) {
    if (d.hour() < 6 || d.hour() > 20 || light > 810) {
      setRelay(RELAYLIGHTPIN, LOW);
    } 
  }

  if (relayLightState == LOW) {
    if (d.hour() > 6 && d.hour() < 20 && light < 750) {
      setRelay(RELAYLIGHTPIN, HIGH);
    } 
  }

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Failed to read");
    lcd.setCursor(0,1);
    lcd.print("from DHT sensor!");
  } else {
    printOnLcd(t, h);

    if (relayHeatState == LOW && t < 20) {
      Serial.println("set relay to high");
      setRelay(RELAYHEATPIN, HIGH);
    }

    if (relayHeatState == HIGH && t > 25) {
      Serial.println("set relay to low");
      setRelay(RELAYHEATPIN, LOW);
    }
  }

  delay(10000);
}
