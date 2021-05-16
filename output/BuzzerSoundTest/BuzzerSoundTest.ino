#include <PushButton.h>
#include <Tone32.hpp>


const int TONE_OUTPUT_PIN = 14;
const int LEFT_BTN_PIN = 15;
PushButton b(LEFT_BTN_PIN);

const int WALL_COLLISION_TONE_FREQUENCY = 100;
const int PLAY_TONE_DURATION_MS = 200;
const int PWM_CHANNEL = 0;    // ESP32 has 16 channels which can generate 16 independent waveforms
const int PWM_FREQ = 500;     // Recall that Arduino Uno is ~490 Hz. Official ESP32 example uses 5,000Hz
const int PWM_RESOLUTION = 8; // We'll use same resolution as Uno (8 bits, 0-255) but ESP32 can go up to 16 bits 
Tone32 _tone32(TONE_OUTPUT_PIN, PWM_CHANNEL);


// https://github.com/bhagman/Tone#musical-notes
#define KEY_C 262  // 261.6256 Hz (middle C)
#define KEY_D 294  // 293.6648 Hz
#define KEY_E 330  // 329.6276 Hz
#define KEY_F 350  // 349.2282 Hz
#define KEY_G 392  // 391.9954 Hz

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
    playWinGameSound();
  }
}

/*
void success() {  tone(TONE_OUTPUT_PIN, 262, 200);
  delay(500);
  tone(TONE_OUTPUT_PIN, 294, 200);
  delay(500);
  tone(TONE_OUTPUT_PIN, 330, 200);
}*/

void playWinGameSound() {
  _tone32.playTone(KEY_C, PLAY_TONE_DURATION_MS);
  delay(PLAY_TONE_DURATION_MS);
  _tone32.playTone(KEY_E, PLAY_TONE_DURATION_MS);
  delay(PLAY_TONE_DURATION_MS);
  _tone32.playTone(KEY_G, PLAY_TONE_DURATION_MS);
  delay(PLAY_TONE_DURATION_MS);
  _tone32.playTone(KEY_E, PLAY_TONE_DURATION_MS);
  delay(PLAY_TONE_DURATION_MS);
  _tone32.playTone(KEY_G, PLAY_TONE_DURATION_MS);
  delay(PLAY_TONE_DURATION_MS);
  _tone32.playTone(0);
}
