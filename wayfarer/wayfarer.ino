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

NewPing leftSonar(TRIG_PIN_LEFT, ECHO_PIN_LEFT, MAX_DISTANCE);
NewPing rightSonar(TRIG_PIN_RIGHT, ECHO_PIN_RIGHT, MAX_DISTANCE);

// Old values before glueing
//ServoWayfarer leftServo(44, 96, 0, 6, 25, -6, -20); // pin, zeroAngle, direction, slow, fast, reverseSlow, reverseFast
//ServoWayfarer rightServo(46, 98, 1, 6, 25, -7, -25);

// Adjusted fast speed and slow
ServoWayfarer leftServo(44, 96, 0, 7, 17, -6, -20); // pin, zeroAngle, direction, slow, fast, reverseSlow, reverseFast
ServoWayfarer rightServo(46, 98, 1, 6, 30, -7, -25);

IRWayfarer irSensor(A0);

LSM303 compass;

const int numReadings = 10;
double IRAvg[numReadings];
double total = 5000;
int iteration = 0;

void setup() {
  Serial.begin(9600);

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

  leftServo.percentSpeed(0);
  rightServo.percentSpeed(0);
  leftServo.attach(44); /// this shouldn't need to be here
  rightServo.attach(46);
  for (int i = 0; i < numReadings; i++) {
    IRAvg[i] = 500;
  }
  pinMode(A0, INPUT);
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
  delay(1000);
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

boolean test = true;

void loop() {
  delay(100);
  leftServo.fast();
  rightServo.fast();



  double irValue = 200;
  while (irValue > 100) {
    delay(25);
    irValue = irSensor.movingAvgIR(IRAvg, &total, &iteration, numReadings);
  }
  delay(385);
  leftServo.zero();
  rightServo.zero();
  delay(500);
  turnLeft90();
  delay(500);

  // IMU
  boolean up = false;
  boolean down = false;
  boolean straight = false;
  boolean mid = false;
  leftServo.fast();
  rightServo.fast();
  delay(1000);
  while (!up || !down || !straight || !mid) {
    compass.read();
    // incline
    if (compass.a.y < -6400 && compass.a.y > -11000) {
      leftServo.fast();
      rightServo.fast();
      up = true;
      // decline
    } else if (compass.a.y > 8900 && compass.a.y < 11000) {
      leftServo.reverseSlow();
      rightServo.reverseSlow();
      down = true;
      // ramp
    } else if (compass.a.y > 600 && compass.a.y < 1500) {
      leftServo.slow();
      rightServo.slow();
      mid = true;
      // ground
    } else if (compass.a.y > 2000 && compass.a.y < 4000) {
      leftServo.slow();
      rightServo.slow();
      if (up && down && mid) {
        straight = true;
      }
    }
    delay(100);
  }

  leftServo.zero();
  rightServo.zero();
  delay(500);
  turnLeft90();
  delay(500);

  while (true) {
    unsigned int left = leftSonar.ping_cm();
    delay(25);
    unsigned int right = rightSonar.ping_cm();
    int average = (left + right) / 2;
    Serial.println(average);
    if (average > 0 && left < 30 && right < 30) {
      leftServo.zero();
      rightServo.zero();
      Serial.println("HIT!");
      while (true) {}
    } else {
      leftServo.fast();
      rightServo.fast();
    }
  }
}
