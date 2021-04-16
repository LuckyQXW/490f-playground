#include "Fader.h"

const int LED1_PIN = 3;
const int LED2_PIN = 5;
const int LED3_PIN = 6;

const int LED1_INTERVAL = 5;
const int LED2_INTERVAL = 5;
const int LED3_INTERVAL = 5;

const int LED1_FADE_AMOUNT = 1;
const int LED2_FADE_AMOUNT = 2;
const int LED3_FADE_AMOUNT = 5;

Fader _led1Fader(LED1_PIN, LED1_INTERVAL, 1);
Fader _led2Fader(LED2_PIN, LED2_INTERVAL, 2);
Fader _led3Fader(LED3_PIN, LED3_INTERVAL, 5);


void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  _led1Fader.update();
  _led2Fader.update();
  _led3Fader.update();
}
