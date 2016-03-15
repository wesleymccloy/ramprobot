#include <Servo.h>
#include <NewPing.h>
#include <Wire.h>
#include <LSM303.h>
#include "ServoWayfarer.h"
#include "IRWayfarer.h"

// General param
#define RUN_MOTORS 0

// Ultrasonic sensors
#define TRIG_PIN_LEFT 9
#define ECHO_PIN_LEFT 8
#define TRIG_PIN_RIGHT 11
#define ECHO_PIN_RIGHT 10
#define TRIG_PIN_SIDE 13
#define ECHO_PIN_SIDE 12
#define MAX_DISTANCE 200

// IR
#define NUMBER_OF_READINGS 20
#define START_VALUE 500

// LEDs
#define LEFT_LED 36
#define RIGHT_LED 40

// Ultrasonic
NewPing leftSonar(TRIG_PIN_LEFT, ECHO_PIN_LEFT, MAX_DISTANCE);
NewPing rightSonar(TRIG_PIN_RIGHT, ECHO_PIN_RIGHT, MAX_DISTANCE);
NewPing sideSonar(TRIG_PIN_SIDE, ECHO_PIN_SIDE, MAX_DISTANCE);

// Servo
// Old values before glueing
//ServoWayfarer leftServo(44, 96, 0, 6, 25, -6, -20); // pin, zeroAngle, direction, slow, fast, reverseSlow, reverseFast
//ServoWayfarer rightServo(46, 98, 1, 6, 25, -7, -25);

// Adjusted values -> will need to change for new motors
ServoWayfarer leftServo(44, 96, 0, 7, 17, -6, -20); // pin, zeroAngle, direction, slow, fast, reverseSlow, reverseFast
ServoWayfarer rightServo(46, 98, 1, 6, 30, -7, -25);
Servo brake; // Test for brake servo

// IMU
LSM303 compass;

// IR
IRWayfarer irSensor(A0);
double IRAvg[NUMBER_OF_READINGS];
double total = NUMBER_OF_READINGS*START_VALUE; // Setting all values to 500
int iteration = 0;

void setup() {
  Serial.begin(9600);

  // Matlab handshake protocol
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
  // Calibration values (need updating)
  compass.m_min = (LSM303::vector<int16_t>) {
    -2093,  -2870,  -3099
  };
  compass.m_max = (LSM303::vector<int16_t>) {
    +3092,  +2093,  +2206
  };

  // Servo
  leftServo.attach(44);
  rightServo.attach(46);
  // test - to remove
  // brake.attach(5);

  leftServo.zero();
  rightServo.zero();

  // IR
  // Set all default values in IR average buffer
  for (int i = 0; i < NUMBER_OF_READINGS; i++) {
    IRAvg[i] = START_VALUE;
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

int gatedValueFilter(int newValue, int* previousValue, int* gatedValue, int* count, int changeThreshold, int howManyInARow) {
  if (abs(newValue - *previousValue) < changeThreshold) {
    count++;
    if (*count >= howManyInARow) {
      *gatedValue = newValue;
    }
  } else {
    *count = 0;
  }
  *previousValue = newValue;
  return *gatedValue;
}

void IRTest() 
// This function turns on the LEDs at the two ranges based on the IR distance.
{
  double irValue = 200;
  bool rightOn = false;
  bool leftOn = false;
  while (true) {
    delay(10);
    irValue = irSensor.movingAvgIR(IRAvg, &total, &iteration, NUMBER_OF_READINGS);
    Serial.println(irValue);
    if (irValue < 50 && !rightOn) {
      digitalWrite(RIGHT_LED, HIGH);
      rightOn = true;
    } else if (irValue > 50 && rightOn) {
      digitalWrite(RIGHT_LED, LOW);
      rightOn = false;
    }
    if (irValue < 100 && !leftOn) {
      digitalWrite(LEFT_LED, HIGH);
      leftOn = true;
    } else if (irValue > 100 && leftOn) {
      digitalWrite(LEFT_LED, LOW);
      leftOn = false;
    }
  }
}

void motorTest() {
  leftServo.slow();
  rightServo.slow();
  delay(2000);
}

void sideUltrasonicTest() {
  int side = MAX_DISTANCE;
  int gated = MAX_DISTANCE;
  int count = 0;
  int previous = MAX_DISTANCE;
  // bool rightOn = false;
  // bool leftOn = false;

  while (true) {
    side = sideSonar.ping_cm();
    delay(50);
    if (side == 0) {
       side = MAX_DISTANCE;
    }
    Serial.println(gatedValueFilter(side, &previous, &gated, &count, 30, 5));  

    // old implementation - delete if new works.
    // it also includes LED indicators
    /*
    if (abs(side - previous) < 30 ) {
      count++;
      if (count >= 5) {
        gatedDefault = side;
        previous = side;
        if (!leftOn) {
          digitalWrite(LEFT_LED, HIGH);
          leftOn = true;
        }
        if (rightOn) {
          digitalWrite(RIGHT_LED, LOW);
          rightOn = false;
        }
      } else {
        previous = side;
        side = gatedDefault;
        if (leftOn) {
        digitalWrite(LEFT_LED, LOW);
        leftOn = false;
        }
        if (!rightOn) {
          digitalWrite(RIGHT_LED, HIGH);
          rightOn = true;
        }
      }
    } else {
      previous = side;
      side = gatedDefault;
      count = 0;
      if (leftOn) {
        digitalWrite(LEFT_LED, LOW);
        leftOn = false;
      }
      if (!rightOn) {
        digitalWrite(RIGHT_LED, HIGH);
        rightOn = true;
      }
    }
    Serial.println(side);
    */
  }
}

void rotateToPost() {
  #if RUN_MOTORS
  leftServo.reverseSlow();
  rightServo.slow();
  #endif
  int delta = MAX_DISTANCE;
  int average = MAX_DISTANCE;
  int left = MAX_DISTANCE;
  int right = MAX_DISTANCE;

  int previous = MAX_DISTANCE; 
  int count = 0;
  int gated = MAX_DISTANCE;
  while (average > 170 || delta > 10) {
    left = leftSonar.ping_cm();
    delay(25); // TODO: is this too frequent?
    right = rightSonar.ping_cm();
    delay(25);
    // a 0 reading means no response
    if (left == 0) {
       left = MAX_DISTANCE;
    }
    if (right == 0) {
      right = MAX_DISTANCE;
    }
    delta = abs(right - left);
    average = (left + right) / 2;
    average = gatedValueFilter(average, &previous, &gated, &count, 30, 5);
    Serial.println(average);
 }
 delay(500);
 leftServo.zero();
 rightServo.zero();
}

void driveToPost() {
  #if RUN_MOTORS
  leftServo.fast();
  rightServo.fast();
  delay(500); // TODO: this may remove acceleration from interfering with acc.
  #endif
  while(true) {
    compass.read();
    if (compass.a.y < 2000 || compass.a.y > 4000) {
      leftServo.zero();
      rightServo.zero();
      break;
    }
    delay(20);
  }
}

void findRamp() {
  leftServo.fast();
  rightServo.fast();

  double irValue = MAX_DISTANCE;
  while (irValue > 50) {
    // TODO fix following
    irValue = irSensor.movingAvgIR(IRAvg, &total, &iteration, NUMBER_OF_READINGS);
    delay(20);
  }
}

void climbRamp() {
  leftServo.fast();
  rightServo.fast();
  delay(200);
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
}

void loop() {
  sideUltrasonicTest();
}
