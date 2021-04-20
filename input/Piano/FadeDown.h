#include <Arduino.h>

class FadeDown {
  private:
    const int _pin;
    const unsigned long _interval;
    
    int _maxOut;
    int _fadeAmount;
    unsigned long _lastToggledTimestampMs;
    int _currBrightness;
    boolean _fading;

  public:
    FadeDown(int pin, unsigned long fadeInterval, int fadeAmount);

    void update();
    void startFade();
    void setToMax();
};
