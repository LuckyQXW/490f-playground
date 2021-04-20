#include "FadeDown.h"

#define KEY_C 262  // 261.6256 Hz (middle C)
#define KEY_D 294  // 293.6648 Hz
#define KEY_E 330  // 329.6276 Hz
#define KEY_F 350  // 349.2282 Hz
#define KEY_G 392  // 391.9954 Hz
#define KEY_A 440 // 440.000 Hz
#define KEY_B 494 // 493.883 Hz
#define KEY_C2 523 // 523.25 Hz

const int OUTPUT_PIEZO_PIN = 2;
const int BUTTON_PIN1 = 5;
const int BUTTON_PIN2 = 6;
const int BUTTON_PIN3 = 7;
const int BUTTON_PIN4 = 8;
const int BUTTON_PIN5 = 9;
const int BUTTON_PIN6 = 10;
const int BUTTON_PIN7 = 11;
const int BUTTON_PIN8 = 12;
const int OUTPUT_LED_PIN = LED_BUILTIN;
const int FADE_LED_PIN = 3;

boolean _buttonsAreActiveLow = true;
FadeDown _ledFadeDown(FADE_LED_PIN, 5, 2);

void setup() {
  // put your setup code here, to run once:
  pinMode(OUTPUT_PIEZO_PIN, OUTPUT);
  pinMode(OUTPUT_LED_PIN, OUTPUT);
  pinMode(FADE_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN1, INPUT_PULLUP);
  pinMode(BUTTON_PIN2, INPUT_PULLUP);
  pinMode(BUTTON_PIN3, INPUT_PULLUP);
  pinMode(BUTTON_PIN4, INPUT_PULLUP);
  pinMode(BUTTON_PIN5, INPUT_PULLUP);
  pinMode(BUTTON_PIN6, INPUT_PULLUP);
  pinMode(BUTTON_PIN7, INPUT_PULLUP);
  pinMode(BUTTON_PIN8, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (pressed(BUTTON_PIN1)) {
    tone(OUTPUT_PIEZO_PIN, KEY_C);
    Serial.println("playing C");
  } else if (pressed(BUTTON_PIN2)) {
    tone(OUTPUT_PIEZO_PIN, KEY_D);
    Serial.println("playing D");
  } else if (pressed(BUTTON_PIN3)) {
    tone(OUTPUT_PIEZO_PIN, KEY_E);
    Serial.println("playing E");
  } else if (pressed(BUTTON_PIN4)) {
    tone(OUTPUT_PIEZO_PIN, KEY_F);
    Serial.println("playing F");
  } else if (pressed(BUTTON_PIN5)) {
    tone(OUTPUT_PIEZO_PIN, KEY_G);
    Serial.println("playing G");
  } else if (pressed(BUTTON_PIN6)) {
    tone(OUTPUT_PIEZO_PIN, KEY_A);
    Serial.println("playing A");
  } else if (pressed(BUTTON_PIN7)) {
    tone(OUTPUT_PIEZO_PIN, KEY_B);
    Serial.println("playing B");
  } else if (pressed(BUTTON_PIN8)) {
    tone(OUTPUT_PIEZO_PIN, KEY_C2);
    Serial.println("playing C2");
  } else {
    noTone(OUTPUT_PIEZO_PIN);
    digitalWrite(OUTPUT_LED_PIN, LOW);
    _ledFadeDown.startFade();
  }
  _ledFadeDown.update();
}

boolean pressed(int pin) {
  int btnVal = digitalRead(pin);
  Serial.println(btnVal);
  if (_buttonsAreActiveLow && btnVal == LOW) {
    digitalWrite(OUTPUT_LED_PIN, HIGH);
    _ledFadeDown.setToMax();
    return true;
  } else if (!_buttonsAreActiveLow && btnVal == HIGH) {
    digitalWrite(OUTPUT_LED_PIN, HIGH);
    _ledFadeDown.setToMax();
    return true;
  }
  return false;
}
