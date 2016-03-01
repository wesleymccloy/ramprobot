#include <Servo.h>

Servo leftServo;
Servo rightServo;

void setup() {
  leftServo.attach(44); // new left
  rightServo.attach(46);
  leftServo.write(96);
  rightServo.write(98);
  delay(5000);
}

int pos = 0;
void loop() {
  leftServo.write(89);
  rightServo.write(104);
  delay(1000);
}
