#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <MovingAverageFilter.hpp>

Adafruit_LIS3DH lis = Adafruit_LIS3DH();
const int DELAY_MS = 5;
const float FRACTION = 100;
const int MULTIPLIER = 100;

const int SMOOTHING_WINDOW_SIZE = 30;

float _xBaseLine = 0;
float _yBaseLine = 0;
float _zBaseLine = 0;

MovingAverageFilter _movingAverageFilterX(SMOOTHING_WINDOW_SIZE);
MovingAverageFilter _movingAverageFilterY(SMOOTHING_WINDOW_SIZE);
MovingAverageFilter _movingAverageFilterZ(SMOOTHING_WINDOW_SIZE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1) yield();
  }

  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  sensors_event_t event;
  lis.getEvent(&event);

  // Multiply by a multiplier to use the integer-based moving average filter
  float x = event.acceleration.x*MULTIPLIER;
  float y = event.acceleration.y*MULTIPLIER;
  float z = event.acceleration.z*MULTIPLIER;

  _movingAverageFilterX.add(x);
  _movingAverageFilterY.add(y);
  _movingAverageFilterZ.add(z);

  Serial.print(_movingAverageFilterX.getAverage()/FRACTION);
  Serial.print(",");
  Serial.print(_movingAverageFilterY.getAverage()/FRACTION);
  Serial.print(",");
  Serial.println(_movingAverageFilterZ.getAverage()/FRACTION);
  
  delay(DELAY_MS);
}
