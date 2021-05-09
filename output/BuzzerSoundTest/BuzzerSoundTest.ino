#include <PushButton.h>

const int WALL_COLLISION_TONE_FREQUENCY = 100;
const int PLAY_TONE_DURATION_MS = 200;
const int TONE_OUTPUT_PIN = 6;
const int LEFT_BTN_PIN = 5;
PushButton b(LEFT_BTN_PIN);

void setup() {
  // put your setup code here, to run once:
  pinMode(LEFT_BTN_PIN, INPUT_PULLUP);
  pinMode(TONE_OUTPUT_PIN, OUTPUT);
  b.setActiveLogic(LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  b.update();
  if (b.isClicked()) {
    tone(TONE_OUTPUT_PIN, 300, 200);
    delay(300);
  }
}

void success() {
  tone(TONE_OUTPUT_PIN, 262, 200);
  delay(500);
  tone(TONE_OUTPUT_PIN, 294, 200);
  delay(500);
  tone(TONE_OUTPUT_PIN, 330, 200);
}

void missBall() {
  tone(TONE_OUTPUT_PIN, 100, 200);
  delay(300);
  tone(TONE_OUTPUT_PIN, 100, 200);
}
