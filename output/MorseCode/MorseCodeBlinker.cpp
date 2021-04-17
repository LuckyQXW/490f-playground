#include "MorseCodeBlinker.h"

MorseCodeBlinker::MorseCodeBlinker(const int pin, const unsigned long interval)
  : _pin(pin), _interval(interval), _messageCode{1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0} {
  _state = LOW;
  _lastToggledTimestamp = 0;
  pinMode(_pin, OUTPUT);
  _index = 0;
}

void MorseCodeBlinker::update() {
  
  unsigned long currentTimestampMs = millis();

  if (currentTimestampMs - _lastToggledTimestamp >= _interval) {
    _lastToggledTimestamp = currentTimestampMs;
    if (_messageCode[_index] == 1) {
      _state = HIGH;
    } else {
      _state = LOW;
    }
    _index++;
    if (_index == sizeof(_messageCode)) {
     _index = 0;
    }
    digitalWrite(_pin, _state);
  }
}
