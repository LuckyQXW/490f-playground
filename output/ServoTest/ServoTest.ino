#include <Servo.h>

Servo _servo;
const int SERVO_PIN = 9;
int angle = 0;

void setup() {
  // put your setup code here, to run once:
  _servo.attach(SERVO_PIN);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Tell the servo to go to a particular angle:
  // Sweep from 0 to 180 degrees:
  for (angle = 0; angle <= 180; angle += 1) {
    _servo.write(angle);
    delay(15);
  }

  // And back from 180 to 0 degrees:
  for (angle = 180; angle >= 0; angle -= 1) {
    _servo.write(angle);
    delay(30);
  }
}
