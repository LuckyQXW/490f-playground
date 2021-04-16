#include <Arduino.h>

class Blinker {
  private:
    const int _pin;
    const unsigned long _interval;

    int _state;
    unsigned long _lastToggledTimestamp;

  public:
    Blinker(int pin, unsigned long blinkInterval);

    void update();
};
