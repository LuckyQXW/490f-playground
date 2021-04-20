#include "FadeDown.h"

FadeDown::FadeDown(const int pin, const unsigned long fadeInterval, int fadeAmount) : _pin(pin), _interval(fadeInterval), _fadeAmount(fadeAmount){
  _lastToggledTimestampMs = 0;
  _currBrightness = 0;
  pinMode(_pin, OUTPUT);
}

void FadeDown::update() {
  unsigned long currentTimestampMs = millis();
  if (currentTimestampMs - _lastToggledTimestampMs >= _interval && _currBrightness >= 0) {
      _lastToggledTimestampMs = currentTimestampMs; // store current time as toggle time
      analogWrite(_pin, _currBrightness);
      _currBrightness -= _fadeAmount;
  }
}

void FadeDown::setBrightness(const int brightness) {
  _currBrightness = brightness;
}

void FadeDown::setLastToggledTimestampMs(const unsigned long lastToggle) {
  _lastToggledTimestampMs = lastToggle;
}
