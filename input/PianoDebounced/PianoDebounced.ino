#include <PushButton.h>

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

PushButton b1(BUTTON_PIN1);
PushButton b2(BUTTON_PIN2);
PushButton b3(BUTTON_PIN3);
PushButton b4(BUTTON_PIN4);
PushButton b5(BUTTON_PIN5);
PushButton b6(BUTTON_PIN6);
PushButton b7(BUTTON_PIN7);
PushButton b8(BUTTON_PIN8);

void setup() {
  // put your setup code here, to run once:
  pinMode(OUTPUT_PIEZO_PIN, OUTPUT);
  pinMode(OUTPUT_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN1, INPUT_PULLUP);
  pinMode(BUTTON_PIN2, INPUT_PULLUP);
  pinMode(BUTTON_PIN3, INPUT_PULLUP);
  pinMode(BUTTON_PIN4, INPUT_PULLUP);
  pinMode(BUTTON_PIN5, INPUT_PULLUP);
  pinMode(BUTTON_PIN6, INPUT_PULLUP);
  pinMode(BUTTON_PIN7, INPUT_PULLUP);
  pinMode(BUTTON_PIN8, INPUT_PULLUP);
  b1.setActiveLogic(LOW);
  b2.setActiveLogic(LOW);
  b3.setActiveLogic(LOW);
  b4.setActiveLogic(LOW);
  b5.setActiveLogic(LOW);
  b6.setActiveLogic(LOW);
  b7.setActiveLogic(LOW);
  b8.setActiveLogic(LOW);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  b1.update();
  b2.update();
  b3.update();
  b4.update();
  b5.update();
  b6.update();
  b7.update();
  b8.update();
  if (b1.isActive()) {
    tone(OUTPUT_PIEZO_PIN, KEY_C);
    Serial.println("playing C");
  } else if (b2.isActive()) {
    tone(OUTPUT_PIEZO_PIN, KEY_D);
    Serial.println("playing D");
  } else if (b3.isActive()) {
    tone(OUTPUT_PIEZO_PIN, KEY_E);
    Serial.println("playing E");
  } else if (b4.isActive()) {
    tone(OUTPUT_PIEZO_PIN, KEY_F);
    Serial.println("playing F");
  } else if (b5.isActive()) {
    tone(OUTPUT_PIEZO_PIN, KEY_G);
    Serial.println("playing G");
  } else if (b6.isActive()) {
    tone(OUTPUT_PIEZO_PIN, KEY_A);
    Serial.println("playing A");
  } else if (b7.isActive()) {
    tone(OUTPUT_PIEZO_PIN, KEY_B);
    Serial.println("playing B");
  } else if (b8.isActive()) {
    tone(OUTPUT_PIEZO_PIN, KEY_C2);
    Serial.println("playing C2");
  } else {
    noTone(OUTPUT_PIEZO_PIN);
    digitalWrite(OUTPUT_LED_PIN, LOW);
  }
}
