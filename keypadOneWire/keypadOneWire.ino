#include <LiquidCrystal_I2C.h>
#include <OnewireKeypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

char KEYS[] = {
  '1', '2', '3', 'A',
  '4', '5', '6', 'B',
  '7', '8', '9', 'C',
  '*', '0', '#', 'D'
};

OnewireKeypad <Print, 16> KP2(Serial, KEYS, 4, 4, A0, 4700, 1000);

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Press a key!");
  lcd.setCursor(0, 1);
  lcd.print("You pressed: ");
  KP2.SetKeypadVoltage(5.0);
  KP2.SetHoldTime(500);
}
void loop() {
  if (char key = KP2.Getkey()) {
    lcd.setCursor(12, 1);
    lcd.print(key);
  }
}