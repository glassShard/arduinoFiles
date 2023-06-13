const int buzzer = 5;

void setup() {
  pinMode(buzzer, OUTPUT);

}

void loop() {
  tone(buzzer, 500);
  delay(1000);
  noTone(buzzer);
  delay(1000);
}
