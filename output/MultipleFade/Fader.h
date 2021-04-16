#include <Arduino.h>

class Fader {
  private:
    const int _pin;
    const unsigned long _interval;
    
    int _fadeAmount;
    int _maxOut;
    int _state;
    unsigned long _lastToggledTimestampMs;
    int _currBrightness;

  public:
    Fader(int pin, unsigned long fadeInterval, int fadeAmount);

    void update();
};
