

#include <LiquidCrystal.h>

/* ---------------- PIN DEFINITIONS ---------------- */
// Inputs
const uint8_t PIN_BTN_MOTOR    = 13; // Motor start button
const uint8_t PIN_BTN_SEATBELT = 3;  // Seatbelt button
const uint8_t PIN_SW_DOOR      = 4;  // Door switch
const uint8_t PIN_LM35         = A0; // Temperature sensor
const uint8_t PIN_LDR          = A1; // LDR light sensor
const uint8_t PIN_FUEL         = A2; // Potentiometer (fuel)

// Outputs
const uint8_t PIN_LED_SEATBELT  = 6;  // Red LED
const uint8_t PIN_LED_HEADLIGHT = 7;  // Blue LED
const uint8_t PIN_LED_FUEL      = 8;  // Yellow LED
const uint8_t PIN_RGB_R         = 9;  // RGB Red (Pink LED)
const uint8_t PIN_RGB_B         = 10; // RGB Blue (to create Pink)
const uint8_t PIN_BUZZER        = 11; // Buzzer
const uint8_t PIN_MOTOR_ENGINE  = 5;  // DC Motor (engine)
const uint8_t PIN_MOTOR_FAN     = 12; // DC Motor (fan)

// LCD Pins
const uint8_t PIN_LCD_RS = 22;
const uint8_t PIN_LCD_EN = 23;
const uint8_t PIN_LCD_D4 = 24;
const uint8_t PIN_LCD_D5 = 25;
const uint8_t PIN_LCD_D6 = 26;
const uint8_t PIN_LCD_D7 = 27;

LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

/* ---------------- GLOBAL VARIABLES ---------------- */
bool engineStarted = false;
bool seatbeltOn = false;
bool doorClosed = true;

/* ---------------- SETUP ---------------- */
void setup() {
  pinMode(PIN_BTN_MOTOR, INPUT_PULLUP);
  pinMode(PIN_BTN_SEATBELT, INPUT_PULLUP);
  pinMode(PIN_SW_DOOR, INPUT_PULLUP);

  pinMode(PIN_LED_SEATBELT, OUTPUT);
  pinMode(PIN_LED_HEADLIGHT, OUTPUT);
  pinMode(PIN_LED_FUEL, OUTPUT);
  pinMode(PIN_RGB_R, OUTPUT);
  pinMode(PIN_RGB_B, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_MOTOR_ENGINE, OUTPUT);
  pinMode(PIN_MOTOR_FAN, OUTPUT);

  lcd.begin(16, 2);
  lcd.clear();
}

/* ---------------- LOOP ---------------- */
void loop() {
  updateInputs();
  handleDoorStatus();
  handleSeatbeltAndEngine();
  handleTemperature();
  handleHeadlights();
  handleFuelLevel();
  if(engineStarted){
    lcd.clear();
    lcd.print("motor calisiyor.......");
    //delay(10000); deneme
  }
}

/* ---------------- FUNCTIONS ---------------- */

void updateInputs() {
  seatbeltOn = digitalRead(PIN_BTN_SEATBELT) == LOW;
  doorClosed = digitalRead(PIN_SW_DOOR) == LOW;
}

void handleSeatbeltAndEngine() {
  if (digitalRead(PIN_BTN_MOTOR) == LOW) { // Motor button pressed
    delay(200); // debounce
    if (!doorClosed) return;

    if (!seatbeltOn) {
      digitalWrite(PIN_BUZZER, HIGH);
      digitalWrite(PIN_LED_SEATBELT, HIGH);
      lcd.clear();
      lcd.print("Emniyet Kemeri\nTakili Degil!");
      engineStarted = false;
      digitalWrite(PIN_MOTOR_ENGINE, LOW);
    } else {
      digitalWrite(PIN_BUZZER, LOW);
      digitalWrite(PIN_LED_SEATBELT, LOW);
      lcd.clear();
      engineStarted = true;
      digitalWrite(PIN_MOTOR_ENGINE, HIGH);
    }
  }
}

void handleTemperature() {
  int tempValue = analogRead(PIN_LM35);
  float temperature = tempValue * (5.0 / 1023.0) * 100.0; 

  if (temperature >= 25.0) {
    digitalWrite(PIN_MOTOR_FAN, HIGH);
    lcd.clear();
    lcd.print("Sicaklik: ");
    lcd.print((int)temperature);
    lcd.print((char)223); // degree symbol
    lcd.print("C\nKlima Acildi");
    delay(100);
  } else {
    digitalWrite(PIN_MOTOR_FAN, LOW);
    
  }
}

void handleHeadlights() {
  int ldrValue = analogRead(PIN_LDR);

  if (ldrValue <= 250) { // dark
    digitalWrite(PIN_LED_HEADLIGHT, HIGH);
    lcd.clear();
    lcd.print("Farlar Açık! ");
    lcd.print(ldrValue);
  } else { // bright
    digitalWrite(PIN_LED_HEADLIGHT, LOW);
    lcd.clear();
    lcd.print("Farlar Kapalı ");
    lcd.print(ldrValue);
  }
}

void handleFuelLevel() {
  int fuelRaw = analogRead(PIN_FUEL);
  int fuelPercent = map(fuelRaw, 0, 1023, 0, 10);

  if (fuelPercent <= 0) {
    digitalWrite(PIN_MOTOR_ENGINE, LOW);
    digitalWrite(PIN_LED_SEATBELT, LOW);
    digitalWrite(PIN_LED_HEADLIGHT, LOW);
    digitalWrite(PIN_LED_FUEL, LOW);
    digitalWrite(PIN_RGB_R, LOW);
    digitalWrite(PIN_RGB_B, LOW);
    lcd.clear();
    lcd.print("Yakit Bitti\nMotor Durdu");
    engineStarted = false;
    return;
  }

  if (fuelPercent <= 5) {
    blinkLED(PIN_LED_FUEL);
    lcd.clear();
    lcd.print("Kritik: Yakit\nCok Az - ");
    lcd.print(fuelPercent);
    lcd.print("%");
  } else if (fuelPercent <= 10) {
    digitalWrite(PIN_LED_FUEL, HIGH);
    lcd.clear();
    lcd.print("Uyari: Yakit\nDusuk - ");
    lcd.print(fuelPercent);
    lcd.print("%");
  } else {
    digitalWrite(PIN_LED_FUEL, LOW);
  }
}

void handleDoorStatus() {
  if (!doorClosed) {
    digitalWrite(PIN_RGB_R, HIGH);
    digitalWrite(PIN_RGB_B, HIGH); // Pink = Red + Blue
    lcd.clear();
    lcd.print("Uyari: Kapi\nAcik!");
    engineStarted = false;
    digitalWrite(PIN_MOTOR_ENGINE, LOW);
  } else {
    digitalWrite(PIN_RGB_R, LOW);
    digitalWrite(PIN_RGB_B, LOW);
  }
}

void blinkLED(uint8_t pin) {
  static unsigned long lastBlink = 0;
  static bool ledState = false;

  if (millis() - lastBlink > 500) {
    ledState = !ledState;
    digitalWrite(pin, ledState ? HIGH : LOW);
    lastBlink = millis();
  }
}