#include <Servo.h>

Servo _servo;
const int SERVO_PIN = 9;
int _angle = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  _servo.attach(SERVO_PIN);
  _servo.write(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    String angleData = Serial.readStringUntil('\n');
    int targetAngle = angleData.toInt();
    targetAngle = constrain(targetAngle, 0, 180);
    while (_angle != targetAngle) {
      if (_angle > targetAngle) {
        _angle--;
      } else {
        _angle++;
      }
      _servo.write(_angle);
      delay(20);
    }
    Serial.print("received ");
    Serial.println(angleData);
  }
}
