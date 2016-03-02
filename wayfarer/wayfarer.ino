#include <Servo.h>
#include "ServoWayfarer.h"

ServoWayfarer leftServo(44,96,0);
ServoWayfarer rightServo(46,98,1);

#define irSense A0; 
int distance = 0;
double distanceToRead = 0.0; 

void setup() {
  Serial.begin(9600); 
}

int pos = 0;

void loop() {
  
  distance = analogRead(irSense); 
  distanceToRead = (2061.19) / pow(distance, 0.983284);
  if ( (distanceToRead < 4) || (distanceToRead > 30) )
  {  
    leftServo.percentSpeed(100);
    rightServo.percentSpeed(100);
  }
  
  else
  {
    leftServo.percentSpeed(0); 
    rightServo.percentSpeed(0); 
  }
  
  delay(200);
  
}
