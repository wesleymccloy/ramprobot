#include <NewPing.h>

/* 
 *  VCC is 5V
 */

#define TRIG_PIN_LEFT 9
#define ECHO_PIN_LEFT 8
#define TRIG_PIN_RIGHT 11
#define ECHO_PIN_RIGHT 10
#define MAX_DISTANCE 200

NewPing leftSonar(TRIG_PIN_LEFT, ECHO_PIN_LEFT, MAX_DISTANCE);
NewPing rightSonar(TRIG_PIN_RIGHT, ECHO_PIN_RIGHT, MAX_DISTANCE);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

int threeInARowDelta = 100;
int threeInARowAverage = 0;
int previousDelta = 0;
int previousAverage = 0;
int counterDelta = 0;
int counterAverage = 0;

void loop() {
  // put your main code here, to run repeatedly:
   delay(50);
   unsigned int left = leftSonar.ping_cm();
//   Serial.print("left: ");
//   Serial.print(uS);
//   Serial.print("cm ");
   delay(50);
   unsigned int right = rightSonar.ping_cm();
   Serial.print("left: ");
   Serial.print(left);
   Serial.print(" right: ");
   Serial.println(right);  
//   int delta = left-right;
//   int average = (left + right)/2;
//   if (previousAverage == average) {
//    counterAverage++;
//    if (counterAverage >3) {
//      counterAverage = 0;
//      if (threeInARowAverage == previousAverage) {
//      } else {
//        threeInARowAverage = previousAverage;
//        Serial.print("average: ");
//        Serial.print(average);
//        Serial.print(" delta: ");
//        Serial.println(delta);
//      }
//    }
//   } else {
//    previousAverage = average;
//    counterAverage = 0;
//   }
//   if (previousDelta == delta) {
//    counterDelta++;
//    if (counterDelta >3) {
//      counterDelta = 0;
//      if (threeInARowDelta == previousDelta) {
//      } else {
//        threeInARowDelta = previousDelta;
//        Serial.print("average: ");
//        Serial.print(average);
//        Serial.print(" delta: ");
//        Serial.println(delta);
//      }
//    }
//   } else {
//    previousDelta = delta;
//    counterDelta = 0;
//   }

   
//   Serial.print("right: ");
//   Serial.print(uS);
//   Serial.println("cm ");
}


// The following code is for without the NewPing Library

//#include <NewPing.h>
//
//#define TRIG_PIN 11
//#define ECHO_PIN 12
//#define MAX_DISTANCE 200
//
//
//NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); 
//
//void setup() {
//  Serial.begin(9600);
////  pinMode(TRIG_PIN, OUTPUT);
////  pinMode(ECHO_PIN, INPUT);
//
//}
//
//void loop() {
//  // put your main code here, to run repeatedly:
////  digitalWrite(TRIG_PIN, LOW);
////  delayMicroseconds(5);
////  digitalWrite(TRIG_PIN, HIGH);
////  delayMicroseconds(10);
////  digitalWrite(TRIG_PIN, LOW);
////
////  pinMode(echoPin, INPUT);
////  duration = pulseIn(echoPin, HIGH);
//}
