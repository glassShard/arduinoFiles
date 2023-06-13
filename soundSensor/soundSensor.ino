int soundDetectedPin = 11;
int soundDetectedVal;
boolean bAlarm = false;
unsigned long lastSoundDetectTime;
int soundAlarmTime = 500;
int ledPin = 8;

void setup() {
  Serial.begin(9600);
  pinMode(soundDetectedPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  soundDetectedVal = digitalRead(soundDetectedPin);
  if (soundDetectedVal == HIGH) {
    digitalWrite(ledPin, HIGH);
    lastSoundDetectTime = millis();
    if (!bAlarm) {
      Serial.println("LOUD");
      bAlarm = true;
    }
  }
  if ((millis() - lastSoundDetectTime) > soundAlarmTime && bAlarm) {
    Serial.println("quiet");
    digitalWrite(ledPin, LOW);
    bAlarm = false;
  }
  delay(10);
}
