/*
 * Adapted from multi-rate blinking example by Jon E. Froehlich
 * https://github.com/makeabilitylab/arduino/tree/master/Basics/digitalWrite/BlinkMultipleWithExternalClass
 */
#include "Blinker.h"

const int LED1_PIN = 3;
const int LED2_PIN = 5;
const int LED3_PIN = 6;

const int LED1_INTERVAL = 1000;
const int LED2_INTERVAL = 550;
const int LED3_INTERVAL = 500;

Blinker _led1Blinker(LED1_PIN, LED1_INTERVAL);
Blinker _led2Blinker(LED2_PIN, LED2_INTERVAL);
Blinker _led3Blinker(LED3_PIN, LED3_INTERVAL);


void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  _led1Blinker.update();
  _led2Blinker.update();
  _led3Blinker.update();
}
