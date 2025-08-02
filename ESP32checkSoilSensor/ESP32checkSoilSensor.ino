#define TIME_TO_SLEEP 10

const int SensorPin = 32;
int minValue = 700;
int maxValue = 2500;
int readValue;
int mappedValue;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
}

void loop() {
  readValue = analogRead(SensorPin);
  mappedValue = map(readValue, minValue, maxValue, 100, 0);
  if (mappedValue > 100) {
    mappedValue = 100;
  }
  if (mappedValue < 0) {
    mappedValue = 0;
  }

  Serial.print("readValue: ");
  Serial.println(readValue);

  Serial.print("mappedValue: ");
  Serial.println(mappedValue);

  sleep(TIME_TO_SLEEP)
}
