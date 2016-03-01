#include <Servo.h>
#include "ServoWayfarer.h"

ServoWayfarer leftServo(44,96,0);
ServoWayfarer rightServo(46,98,1);

void setup() {
}

int pos = 0;
void loop() {
leftServo.percentSpeed(100);
rightServo.percentSpeed(100);
delay(1000);
}
