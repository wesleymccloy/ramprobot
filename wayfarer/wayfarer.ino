#include <Servo.h>
#include <NewPing.h>
#include <Wire.h>
#include <LSM303.h>
#include "ServoWayfarer.h"
#include "IRWayfarer.h"

#define TRIG_PIN_LEFT 9
#define ECHO_PIN_LEFT 8
#define TRIG_PIN_RIGHT 11
#define ECHO_PIN_RIGHT 10
#define MAX_DISTANCE 200

#define LEFT_LED 42
#define RIGHT_LED 40


NewPing leftSonar(TRIG_PIN_LEFT, ECHO_PIN_LEFT, MAX_DISTANCE);
NewPing rightSonar(TRIG_PIN_RIGHT, ECHO_PIN_RIGHT, MAX_DISTANCE);

// Old values before glueing
//ServoWayfarer leftServo(44, 96, 0, 6, 25, -6, -20); // pin, zeroAngle, direction, slow, fast, reverseSlow, reverseFast
//ServoWayfarer rightServo(46, 98, 1, 6, 25, -7, -25);

// Adjusted fast speed and slow
ServoWayfarer leftServo(44, 96, 0, 7, 17, -6, -20); // pin, zeroAngle, direction, slow, fast, reverseSlow, reverseFast
ServoWayfarer rightServo(46, 98, 1, 6, 30, -7, -25);

Servo brake;

IRWayfarer irSensor(A0);

LSM303 compass;

const int numReadings = 10;
double IRAvg[numReadings];
double total = 5000;
int iteration = 0;

void setup() {
  Serial.begin(9600);

  // Matlab comms
  //  Serial.println('a');
  //  char a = 'b';
  //  while (a != 'a')
  //  {
  //    a = Serial.read();
  //  }

  // IMU
  Wire.begin();
  compass.init();
  compass.enableDefault();
  compass.m_min = (LSM303::vector<int16_t>) {
    -2093,  -2870,  -3099
  };
  compass.m_max = (LSM303::vector<int16_t>) {
    +3092,  +2093,  +2206
  };

  leftServo.zero();
  rightServo.zero();
  leftServo.attach(44); /// this shouldn't need to be here
  rightServo.attach(46);
  
  brake.attach(5);
  
  for (int i = 0; i < numReadings; i++) {
    IRAvg[i] = 500;
  }
  pinMode(A0, INPUT);

  // LEDs
  pinMode(LEFT_LED, OUTPUT);
  pinMode(RIGHT_LED, OUTPUT);

  digitalWrite(LEFT_LED, LOW);
  digitalWrite(RIGHT_LED, LOW);

  delay(50);
}

void turnLeft90() {
  Serial.println("start of turnLeft");
  leftServo.zero();
  rightServo.zero();
  delay(500);
  compass.read();
  float x = compass.heading();
  float y = ((int)x + 270) % 360;
  Serial.print("x: ");
  Serial.print(x);
  Serial.print(" y: ");
  Serial.println(y);

  leftServo.reverseSlow();
  rightServo.slow();
  delay(500);
  compass.read();
  float current = compass.heading();
  Serial.print("current: ");
  Serial.println(current);
  while (((x < 90) && (current < x || current > y)) || ((x >= 90) && (current < x) && (current > y ))) {
    delay(20);
    compass.read();
    current = compass.heading();
    Serial.print("current: ");
    Serial.println(current);
  }
  Serial.println("end of turnleft");
  leftServo.zero();
  rightServo.zero();
}

void shittyTurn90() {
  leftServo.reverseSlow();
  rightServo.slow();
  delay(1000);
  leftServo.zero();
  leftServo.zero();
}

boolean test = true;

void loop() {

  // Simple motor test
    brake.write(30);
    
//    rightServo.slow();
    delay(2000);
    brake.write(180);
    delay(2000);


  // To test Ultrasonic
  //    uint8_t left = leftSonar.ping_cm();
  //    if (left == 0) {
  //      left = MAX_DISTANCE;
  //    }
  //    Serial.println(left);
  //    delay(50);

  
//  // To find post
//  //    delay(1000);
//  //    leftServo.reverseSlow();
//  //    rightServo.slow();
//  int delta = 100;
//  int average = 200;
//  uint8_t left = 200;
//  uint8_t right = 200;
////  uint8_t runningAverage[3] = { MAX_DISTANCE, MAX_DISTANCE, MAX_DISTANCE};
////  int total = MAX_DISTANCE * 3;
////  int i = 0;
//  int previous = MAX_DISTANCE; 
//  int count = 0;
//  int gatedDefault = 200;
//  while (true) { //average > 170 || delta > 5) {
//    left = leftSonar.ping_cm();
//    delay(25);
////    right = rightSonar.ping_cm();
////    delay(25);
//    if (left == 0) {
//      left = MAX_DISTANCE;
//    }
////    if (right == 0) {
////      right = MAX_DISTANCE;
////    }
////    delta = abs(right - left);
////    average = (left + right) / 2;
//      average = left;
//    if (abs(average - previous) < 30 ){//&& delta <= 10) {
//        count++;
//        if (count >= 5) {
//          gatedDefault = average;
//          previous = average;
//        } else {
//          previous = average;
//          average = gatedDefault;
//        }
//    } else {
//      previous = average;
//      average = gatedDefault;
//      count = 0;
//    }
//    Serial.println(average);
////    total = total - runningAverage[i];
////    runningAverage[i] = average;
////    total = total + average;
//    
//    //    Serial.print("left: ");
//    //    Serial.print(left);
//    //    Serial.print(" right: ");
//    //    Serial.print(right);
//    //    Serial.print(" average: ");
//    //    Serial.print(average);
//    //    Serial.print(" delta: ");
//    //    Serial.println(delta);
//  }

  //
  //  Serial.println("POST DETECTED!");
  //  // Post detected
  //  digitalWrite(LEFT_LED, HIGH);
  //  digitalWrite(RIGHT_LED, HIGH);
  //    leftServo.zero();
  //    rightServo.zero();
  //    delay(200);
  //    leftServo.fast();
  //    rightServo.fast();
  //  while (left > 5 && right > 5) {
  //    left = leftSonar.ping_cm();
  //    delay(50);
  //    right = rightSonar.ping_cm();
  //    delay(50);
  //    if (left == 0) {
  //      left = 200;
  //    }
  //    if (right == 0) {
  //      right = 200;
  //    }
  //  }
  //    leftServo.zero();
  //    rightServo.zero();
  //  while (true) {
  //    delay(1000);
  //  }

  // Go up and down ramp

  //  leftServo.fast();
  //  rightServo.fast();
  //
  //  double irValue = 200;
  //  while (irValue > 100) {
  //    delay(25);
  //    irValue = irSensor.movingAvgIR(IRAvg, &total, &iteration, numReadings);
  //  }
  //  delay(385);
  //  leftServo.zero();
  //  rightServo.zero();
  //  delay(500);
  //  shittyTurn90();
  //  delay(500);
  //
  //  // IMU
  //  boolean up = false;
  //  boolean down = false;
  //  boolean straight = false;
  //  boolean mid = false;
  //  leftServo.fast();
  //  rightServo.fast();
  //  delay(1000);
  //  while (!up || !down || !straight || !mid) {
  //    compass.read();
  //    // incline
  //    if (compass.a.y < -6400 && compass.a.y > -11000) {
  //      leftServo.fast();
  //      rightServo.fast();
  //      up = true;
  //      // decline
  //    } else if (compass.a.y > 8900 && compass.a.y < 11000) {
  //      leftServo.reverseSlow();
  //      rightServo.reverseSlow();
  //      down = true;
  //      // ramp
  //    } else if (compass.a.y > 600 && compass.a.y < 1500) {
  //      leftServo.slow();
  //      rightServo.slow();
  //      mid = true;
  //      // ground
  //    } else if (compass.a.y > 2000 && compass.a.y < 4000) {
  //      leftServo.slow();
  //      rightServo.slow();
  //      if (up && down && mid) {
  //        straight = true;
  //      }
  //    }
  //    delay(100);
  //  }
  //
  //  leftServo.zero();
  //  rightServo.zero();
  //  delay(500);
  //  shittyTurn90();
  //  delay(500);
  //
  //  while (true) {
  //    unsigned int left = leftSonar.ping_cm();
  //    delay(25);
  //    unsigned int right = rightSonar.ping_cm();
  //    int average = (left + right) / 2;
  //    if (average > 0 && left < 30 && right < 30) {
  //      leftServo.zero();
  //      rightServo.zero();
  //      while (true) {}
  //    } else {
  //      leftServo.fast();
  //      rightServo.fast();
  //    }
  //  }
}
