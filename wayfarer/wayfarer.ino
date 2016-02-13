#include <Servo.h>

Servo leftServo;
Servo rightServo;

void setup() {
  leftServo.attach(4);
  rightServo.attach(7);
}

int pos = 0;
void loop() {
//   for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
//    // in steps of 1 degree
//    leftServo.write(pos);              // tell servo to go to position in variable 'pos'
//    rightServo.write(pos);
//    delay(15);                       // waits 15ms for the servo to reach the position
//  }
//  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//    leftServo.write(pos);              // tell servo to go to position in variable 'pos'
//    rightServo.write(pos);
//    delay(15);                       // waits 15ms for the servo to reach the position
//  }


leftServo.write(180);
rightServo.write(180);
delay(1000);
}
