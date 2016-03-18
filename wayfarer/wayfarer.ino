#include <Servo.h>
#include <NewPing.h>
#include <Wire.h>
#include <LSM303.h>
#include "ServoWayfarer.h"
#include "IRWayfarer.h"

// General param
#define RUN_MOTORS 1
#define DEBUG 0
#define BUTTON 30

// Tweak parameters
#define TURN_ONTO_RAMP_DELAY 600
#define TURN_AT_BASE_DELAY 790
#define TURN_AT_POST_DELAY 650
#define DRIVE_PAST_RAMP_DISTANCE 4

// Fully charged 9V at 8.3 battery at 7.17V - dist = 5 (fast)

// works with slow and dist = 5 - 9V at 8.1 battery at 7.00V
// same as above: turn at base = 740 (700 initially), at post = 680, onto ramp = 600

// fully charged: 7.21V for the battery. 8.21V for the 9V

// 6 worked. base angle too little, dirty wheels maybe
// 8.18V for 9V, battery at 7.09

// works, like half the time
// 7.94V for 9V, battery at 7V

// changing to 7, doesn't go far enough
// jerking forwards a tiny bit

// jerked once
// 7.86V for 9V, 6.96V battery

// shit hit the fan, back to 4 and fast speed, worked twice

// 7.9V for 9V, battery at 

// fully charged. d = 4. failed 3 times, worked once. trying to reproduce

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
#define START_VALUE 200
#define IR_PIN A0

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
ServoWayfarer leftServo(95, 0, 7, 24, -7, -20, -3, 7); // zeroAngle, direction, slow, fast, reverseSlow, reverseFast
ServoWayfarer rightServo(95, 1, 6, 27, -7, -23, 3, 4);

// IMU
LSM303 compass;

// IR
IRWayfarer irSensor(IR_PIN);
double IRAvg[NUMBER_OF_READINGS];
double total = NUMBER_OF_READINGS * START_VALUE; // Setting all values to 500
int iteration = 0;

void setup() {
  Serial.begin(9600);

  // IMU
  Wire.begin();
  compass.init();
  compass.enableDefault();

  // Calibration values
  // Old
  /*
    compass.m_min = (LSM303::vector<int16_t>) {
    -2093,  -2870,  -3099
    };
    compass.m_max = (LSM303::vector<int16_t>) {
    +3092,  +2093,  +2206
    };
  */
  compass.m_min = (LSM303::vector<int16_t>) {
    -2449,  -2818,  -2794
  };
  compass.m_max = (LSM303::vector<int16_t>) {
    +2718,  +2844,  +3387
  };

  // Servo
  leftServo.attach(44);
  rightServo.attach(46);

  leftServo.zero();
  rightServo.zero();

  // IR
  // Set all default values in IR average buffer
  for (int i = 0; i < NUMBER_OF_READINGS; i++) {
    IRAvg[i] = START_VALUE;
  }
  pinMode(IR_PIN, INPUT);

  // LEDs
  pinMode(LEFT_LED, OUTPUT);
  pinMode(RIGHT_LED, OUTPUT);
  digitalWrite(LEFT_LED, LOW);
  digitalWrite(RIGHT_LED, LOW);

  pinMode(BUTTON, INPUT);

  delay(50);
}

int gatedValueFilter(int newValue, int* previousValue, int* gatedValue, int* count, int changeThreshold, int howManyInARow) {

  if (abs(newValue - *previousValue) < changeThreshold) {
    *count = *count + 1;
    if (*count > howManyInARow) {
      *gatedValue = newValue;
    }
  } else {
    *count = 0;
  }
  *previousValue = newValue;
  return *gatedValue;
}

void turnOntoRamp() {
  leftServo.reverseSlow();
  rightServo.slow();
  delay(TURN_ONTO_RAMP_DELAY); // 750 at 10pm, 780 at 11:30
  leftServo.zero();
  rightServo.zero();
}

void turnAtBase() {
  leftServo.reverseSlow();
  rightServo.slow();
  delay(TURN_AT_BASE_DELAY);
  leftServo.zero();
  rightServo.zero();
}
void turnAtPost() {
  leftServo.reverseSlow();
  rightServo.slow();
  delay(TURN_AT_POST_DELAY);
  leftServo.zero();
  rightServo.zero();
}

void driveToPost() {
#if RUN_MOTORS
  leftServo.fast();
  rightServo.fast();
  delay(500); // TODO: this may remove acceleration from interfering with acc.
#endif
  while (true) {
    compass.read();
    if (compass.a.y < -4000 && compass.a.y > -10000) {
#if RUN_MOTORS
      digitalWrite(LEFT_LED, HIGH);
      delay(600);
      leftServo.zero();
      rightServo.zero();
#endif
      break;
    }
    if (compass.a.x < -4000 || compass.a.x > 4000) {
#if RUN_MOTORS
      digitalWrite(RIGHT_LED, HIGH);
      delay(200);
      leftServo.zero();
      rightServo.zero();
#endif
      break;
    }
    delay(20);
  }
}

boolean driveTillPostOnLeft() {
#if RUN_MOTORS
  leftServo.fast();
  rightServo.fast();
  delay(200);
#endif
  int side = MAX_DISTANCE;
  int gated = MAX_DISTANCE;
  int count = 0;
  int previous = MAX_DISTANCE;

  while (side > 170) {
    delay(50);
    side = sideSonar.ping_cm();
    if (side == 0) {
      side = MAX_DISTANCE;
    }
    side = gatedValueFilter(side, &previous, &gated, &count, 30, 5);
    compass.read();
    if (compass.a.y < -4000 && compass.a.y > -10000) {
      digitalWrite(LEFT_LED, HIGH);
      return false;
    } else if (compass.a.x < -4000 || compass.a.x > 4000) {
      digitalWrite(RIGHT_LED, HIGH);
      return false;
    }
#if DEBUG
    Serial.println(side);
    Serial.print(",");
    Serial.print(150);
#endif
  }
#if RUN_MOTORS
  leftServo.zero();
  rightServo.zero();
#endif
  digitalWrite(LEFT_LED, HIGH);
  return true;

}

void driveTillCloseToWall() {

  leftServo.fast();
  rightServo.fast();
  int delta = MAX_DISTANCE;
  int average = MAX_DISTANCE;
  int left = MAX_DISTANCE;
  int right = MAX_DISTANCE;

  int previous = MAX_DISTANCE;
  int count = 0;
  int gated = MAX_DISTANCE;
  while (true) {
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
    if (delta < 10) {
      average = gatedValueFilter(average, &previous, &gated, &count, 30, 3);
    } else {
      average = gated;
    }
#if DEBUG
    Serial.print(average);
    Serial.print(",");
    Serial.println(25);
#endif
    if (average <= 25) {
      break;
    }
  }

#if RUN_MOTORS
  leftServo.zero();
  rightServo.zero();
#endif
}

void climbRamp() {
#if RUN_MOTORS
  leftServo.fast();
  rightServo.fast();
#endif
  delay(200);
  boolean up = false;
  boolean down = false;
  boolean straight = false;
  boolean mid = false;
  boolean wheelie = false;
#if RUN_MOTORS
  leftServo.fast();
  rightServo.fast();
#endif
  delay(1000);
  while (!up || !down || !straight || !mid) {
    compass.read();
    // incline
    if (compass.a.y < -8000 && compass.a.y > -10000) {
#if RUN_MOTORS
      leftServo.fast();
      rightServo.fast();
#endif
      digitalWrite(LEFT_LED, HIGH);
      digitalWrite(RIGHT_LED, LOW);
      up = true;
      // decline
    } else if (compass.a.y > 6000 && compass.a.y < 8000) { // the 5000 may need revising.
#if RUN_MOTORS
      delay(100);
      leftServo.zero();
      rightServo.zero();
#endif
      digitalWrite(LEFT_LED, LOW);
      digitalWrite(RIGHT_LED, HIGH);
      down = true;
      // ramp
    } else if (compass.a.y > -2000 && compass.a.y < 0) {
#if RUN_MOTORS
      leftServo.slowRamp();
      rightServo.slowRamp();
#endif
      digitalWrite(RIGHT_LED, HIGH);
      digitalWrite(LEFT_LED, HIGH);
      mid = true;
      // ground
    } else if (compass.a.y > 0 && compass.a.y < 1000) {
#if RUN_MOTORS
      leftServo.slow();
      rightServo.slow();
#endif
      digitalWrite(RIGHT_LED, LOW);
      digitalWrite(LEFT_LED, LOW);
      if (up && down && mid) {
        straight = true;
      }
    } else if (compass.a.y < -11000 && compass.a.y > -12000 && !wheelie) {
      wheelie = true;
#if RUN_MOTORS
      leftServo.reverseSlow();
      rightServo.reverseSlow();
      delay(200);
#endif
    }
    // roll thing
    //    if (compass.a.x < -2000) {
    //      leftServo.reverseSlow();
    //      rightServo.reverseSlow();
    //      delay(50);
    //      rightServo.slow();
    //      delay(50);
    //
    //    } else if (compass.a.x > 2000) {
    //      leftServo.reverseSlow();
    //      rightServo.reverseSlow();
    //      delay(50);
    //      leftServo.slow();
    //      delay(50);
    //    }
    delay(100);
  }
  leftServo.zero();
  rightServo.zero();
}

void findRamp() {
  leftServo.fast();
  rightServo.fast();
  delay(200); // delay to avoid initial false positives
  double irValue = MAX_DISTANCE;
  while (irValue > 50) {
    delay(5);
    irValue = irSensor.movingAvgIR(IRAvg, &total, &iteration, NUMBER_OF_READINGS);
#if DEBUG
    Serial.print(50);
    Serial.print(",");
    Serial.println(irValue);
#endif

  }
  // delay(225); // do not delete
  leftServo.zero();
  rightServo.zero();
  delay(100);
  int distanceToWall = stabilizedUltrasonicDistanceRunningAverage(10);
  leftServo.fast();
  rightServo.fast();
  exitWhenUltrasonicAt(distanceToWall - DRIVE_PAST_RAMP_DISTANCE, distanceToWall); // half ramp width
  leftServo.zero();
  rightServo.zero();
}

int stabilizedUltrasonicDistanceRunningAverage(int sampleSize) {
  int total = 0;
  int left = MAX_DISTANCE;
  int right = MAX_DISTANCE;
  int average = MAX_DISTANCE;
  for (int i = 0; i < sampleSize; i++) {
    left = leftSonar.ping_cm();
    delay(25);
    right = rightSonar.ping_cm();
    delay(25);
    if (left == 0) {
      left = MAX_DISTANCE;
    }
    if (right == 0) {
      right = MAX_DISTANCE;
    }
    average = (left + right) / 2;
    total += average;
  }
  return total / sampleSize;
}

void exitWhenUltrasonicAt(int distance, int startValue) {
  int delta = MAX_DISTANCE;
  int average = startValue;
  int left = startValue;
  int right = startValue;

  int previous = startValue;
  int count = 0;
  int gated = startValue;
  while (true) {
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
    if (delta < 10) {
      average = gatedValueFilter(average, &previous, &gated, &count, 30, 2);
    } else {
      average = gated;
    }
#if DEBUG
    Serial.print(startValue);
    Serial.print(",");
    Serial.print(distance);
    Serial.print(",");
    Serial.println(average);
    //    Serial.print(average);
    //    Serial.print(",");
    //    Serial.println(distance);
#endif
    if (average <= distance ) {
      return;
    }
  }
}
/*
void climbOntoRamp() {
  // Before being on ramp
  leftServo.fast();
  rightServo.fast();
  delay(100);
  int count = 0;
  compass.read();

  while (true) {
    compass.read();
    if (compass.a.x < -4500) {
      count = 0;
      digitalWrite(LEFT_LED, HIGH);
      rolledRight();
      delay(1000);
      digitalWrite(LEFT_LED, LOW);
    } else if (compass.a.x > 4500) {
      count = 0;
      digitalWrite(RIGHT_LED, HIGH);
      rolledLeft();
      delay(1000);
      digitalWrite(RIGHT_LED, LOW);
    }
    if (compass.a.y < -11000) {
      count = 0;
      digitalWrite(RIGHT_LED, HIGH);
      digitalWrite(LEFT_LED, HIGH);
      wheelied();
      delay(1000);
      digitalWrite(RIGHT_LED, LOW);
      digitalWrite(LEFT_LED, LOW);
    } else if (compass.a.y < -8000) { // incline
      count++;
    }
    if (count >= 20) { // if incline 30 times in a row then all good.
      return;
    }
  }
}

void rolledRight() {
  leftServo.reverseSlow();
  rightServo.reverseSlow();
  delay(1000);
  rightServo.slow();
  delay(200);
  leftServo.fast();
  rightServo.fast();
}

void rolledLeft() {
  leftServo.reverseSlow();
  rightServo.reverseSlow();
  delay(1000);
  leftServo.slow();
  delay(200);
  leftServo.fast();
  rightServo.fast();
}

void wheelied() {
  leftServo.reverseSlow();
  rightServo.reverseSlow();
  delay(1000);
  leftServo.fast();
  rightServo.fast();
}
*/

void loop() {
  while (digitalRead(BUTTON) == LOW) {};
    digitalWrite(LEFT_LED, LOW);
    digitalWrite(RIGHT_LED, LOW);
    delay(1000);
    findRamp();
    delay(500);
    turnOntoRamp();
    delay(500);
    //  climbOntoRamp();
    //  delay(500);
    climbRamp();
    delay(1500); // changed to see if we get better alignment
    driveTillCloseToWall();
    delay(500);
    turnAtBase();
    delay(500);
  
    digitalWrite(RIGHT_LED, LOW);
    digitalWrite(LEFT_LED, LOW);
    if (driveTillPostOnLeft()) {
      // added delay before turn, for better centering
      leftServo.fast();
      rightServo.fast();
      delay(300);
      leftServo.zero();
      rightServo.zero();
      delay(500);
      turnAtPost();
      delay(500);
  
      driveToPost();
    } else {
      leftServo.fast();
      rightServo.fast();
      delay(200);
      leftServo.zero();
      rightServo.zero();
    }
}
