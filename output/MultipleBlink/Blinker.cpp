#include "Blinker.h"

Blinker::Blinker(const int pin, const unsigned long blinkInterval) : _pin(pin), _interval(blinkInterval){
  _state = LOW;
  _lastToggledTimestamp = 0;
  pinMode(_pin, OUTPUT);
}

void Blinker::update() {
  unsigned long currentTimestampMs = millis();

  if (currentTimestampMs - _lastToggledTimestamp >= _interval) {
    _lastToggledTimestamp = currentTimestampMs;
    _state = !_state;
    digitalWrite(_pin, _state);
  }
}
