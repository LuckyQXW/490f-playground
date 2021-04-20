#include <Arduino.h>

class FadeDown {
  private:
    const int _pin;
    const unsigned long _interval;
    
    int _fadeAmount;
    unsigned long _lastToggledTimestampMs;
    int _currBrightness;

  public:
    FadeDown(int pin, unsigned long fadeInterval, int fadeAmount);

    void update();
    void setBrightness(int brightness);
    void setLastToggledTimestampMs(unsigned long lastToggle);
};
