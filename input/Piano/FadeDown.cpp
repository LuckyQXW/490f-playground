#include "FadeDown.h"

FadeDown::FadeDown(const int pin, const unsigned long fadeInterval, int fadeAmount) : _pin(pin), _interval(fadeInterval), _fadeAmount(fadeAmount){
  _lastToggledTimestampMs = 0;
  _currBrightness = 0;
  _maxOut = 255;
  _fading = true;
  pinMode(_pin, OUTPUT);
}

void FadeDown::update() {
  unsigned long currentTimestampMs = millis();
  if (_fading && currentTimestampMs - _lastToggledTimestampMs >= _interval && _currBrightness >= 0) {
      _lastToggledTimestampMs = currentTimestampMs; // store current time as toggle time
      analogWrite(_pin, _currBrightness);
      _currBrightness -= _fadeAmount;
  }
}

void FadeDown::startFade() {
  if (!_fading) {
    _lastToggledTimestampMs = millis();
    _currBrightness = _maxOut;
    _fading = true;
  }
}

void FadeDown::setToMax() {
  _fading = false;
  analogWrite(_pin, _maxOut);
}
