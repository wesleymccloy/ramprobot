#include <Servo.h>
#include <NewPing.h>
#include "ServoWayfarer.h"

#define TRIG_PIN_LEFT 9
#define ECHO_PIN_LEFT 8
#define TRIG_PIN_RIGHT 11
#define ECHO_PIN_RIGHT 10
#define MAX_DISTANCE 200

NewPing leftSonar(TRIG_PIN_LEFT, ECHO_PIN_LEFT, MAX_DISTANCE);
NewPing rightSonar(TRIG_PIN_RIGHT, ECHO_PIN_RIGHT, MAX_DISTANCE);

ServoWayfarer leftServo(44,96,0);
ServoWayfarer rightServo(46,98,1);

void setup() {
  Serial.begin(9600);
  leftServo.percentSpeed(0);
  rightServo.percentSpeed(0);
  leftServo.attach(44);
  rightServo.attach(46);
}

void loop() {

    delay(25);
    unsigned int left = leftSonar.ping_cm();
    delay(25);
    unsigned int right = rightSonar.ping_cm();
    int average = (left+right)/2;
    Serial.println(average);
    if (average > 0 && left < 30 && right < 30) {
          leftServo.percentSpeed(0);
          rightServo.percentSpeed(0);
          Serial.println("HIT!");
          delay(500);
    } else {
      leftServo.percentSpeed(50);
      rightServo.percentSpeed(50);
    }
}
