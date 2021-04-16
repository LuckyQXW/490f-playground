#include "Fader.h"

Fader::Fader(const int pin, const unsigned long fadeInterval, int fadeAmount) : _pin(pin), _interval(fadeInterval), _fadeAmount(fadeAmount){
  _state = LOW;
  _lastToggledTimestampMs = 0;
  _currBrightness = 0;
  _maxOut = 255;
  pinMode(_pin, OUTPUT);
}

void Fader::update() {
  unsigned long currentTimestampMs = millis();
  if (currentTimestampMs - _lastToggledTimestampMs >= _interval) {
    if (_currBrightness < 0 || _currBrightness >= _maxOut) {
        _fadeAmount = -_fadeAmount;
      }
      _lastToggledTimestampMs = currentTimestampMs; // store current time as toggle time
      analogWrite(_pin, _currBrightness);
      _currBrightness += _fadeAmount;
  }
}
