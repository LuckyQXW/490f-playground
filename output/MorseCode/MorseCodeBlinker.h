#include <Arduino.h>

class MorseCodeBlinker {
  private:
    const int _pin;
    const unsigned long _interval;  // controls the size of 1 time unit
    const int _messageCode[34];  // includes morse code for SOS

    int _state;
    unsigned long _lastToggledTimestamp;
    int _index;

  public:
    MorseCodeBlinker(int pin, unsigned long interval);

    void update();
};
