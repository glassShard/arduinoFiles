const int SensorPin = 32;
int readValue;
int minValue = 5000;
int maxValue = 0;
int mappedValue;

void setup() {
  Serial.begin(115200);
}

void loop() {
  readValue = analogRead(SensorPin);
  if (readValue) {
    if (readValue > maxValue) {
      maxValue = readValue;
    }
    if (readValue < minValue) {
      minValue = readValue;
    }
    mappedValue = map(readValue, 973, 2674, 100, 0);
    Serial.print("sensor value = ");
    Serial.print(readValue);
    Serial.print(" min: ");
    Serial.print(minValue);
    Serial.print(" ");
    Serial.print("max: ");
    Serial.print(maxValue);
    Serial.print(" mapped value = ");
    Serial.print(mappedValue);
    Serial.println("%");
  } else {
    Serial.println("No read value");
  }

  delay(5000);
}
