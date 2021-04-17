#include "MorseCodeBlinker.h"

MorseCodeBlinker _led1Blinker(3, 200);

void setup() {
  // put your setup code here, to run once:
  
}

void loop() {
  // put your main code here, to run repeatedly:
  _led1Blinker.update();
}
