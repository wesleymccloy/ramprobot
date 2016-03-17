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
ServoWayfarer leftServo(95, 0, 8, 24, -7, -20, -3, 7); // pin, zeroAngle, direction, slow, fast, reverseSlow, reverseFast
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

  //  Serial.print("newValue: ");
  //  Serial.print(newValue);
  //  Serial.print(" previousValue: ");
  //  Serial.print(*previousValue);
  //  Serial.print(" difference: ");
  //  Serial.print(abs(newValue - *previousValue));
  //  Serial.print(" count: ");
  //  Serial.print(*count);
  //  Serial.print(" gatedValue: ");
  //  Serial.println(*gatedValue);
  *previousValue = newValue;
  return *gatedValue;
}

void turnLeft90() {
  leftServo.zero();
  rightServo.zero();
  delay(500);
  compass.read();
  float x = compass.heading();
  int threshold = 5;
  float y = ((int)x + 270 + threshold) % 360;
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
    delay(10);
    compass.read();
    current = compass.heading();
    Serial.print("current: ");
    Serial.println(current);
  }
  Serial.println("end of turnleft");
  leftServo.zero();
  rightServo.zero();
}

void turnIMUTest() {
  delay(500);
  compass.read();
  float x = compass.heading();
  int threshold = 10;
  float y = ((int)x + 270 + threshold) % 360;
  //  Serial.print("x: ");
  //  Serial.print(x);
  //  Serial.print(" y: ");
  //  Serial.println(y);
  int numReadings = 3;
  float total = x * numReadings;
  float avg[numReadings];
  for (int i = 0; i < numReadings; i++) {
    avg[i] = x;
  }
  int count = 0;
  float previousValue = x;
  float value = x;
  leftServo.reverseSlow();
  rightServo.slow();
  delay(500);
  while (((x < 90) && (value <= x || value > y)) || ((x >= 90) && (value <= x) && (value > y ))) {
    //delay(10);
    total = total - avg[count];
    compass.read();
    value = compass.heading();
    avg[count] = value;
    total = total + value;
    count++;
    if (count >= numReadings) {
      count = 0;
    }
    if (abs(previousValue - value) < 100) {
      value = total / numReadings;
      previousValue = value;
    } else {
      for (int i = 0; i < numReadings; i++) {
        avg[i] = value;
      }
      total = value * numReadings;
    }
    previousValue = value;
    Serial.print(x);
    Serial.print(",");
    Serial.print(y);
    Serial.print(",");
    Serial.println(value);
  }
  leftServo.zero();
  rightServo.zero();
  delay(500);
}

void shittyTurn90() {
  leftServo.reverseSlow();
  rightServo.slow();
  delay(600); // 750 at 10pm, 780 at 11:30
  leftServo.zero();
  rightServo.zero();
}

void shittyTurn902() {
  leftServo.reverseSlow();
  rightServo.slow();
  delay(680);
  leftServo.zero();
  rightServo.zero();
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
    //    Serial.print(50);
    //    Serial.print(",");
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
  leftServo.zero();
  rightServo.zero();
  delay(1000);
  while (digitalRead(BUTTON) == LOW) {};
  leftServo.slow();
  rightServo.slow();
  delay(1000);
  while (digitalRead(BUTTON) == LOW) {};
  leftServo.zero();
  rightServo.zero();
  delay(1000);
  while (digitalRead(BUTTON) == LOW) {};
  leftServo.fast();
  rightServo.fast();
  delay(1000);
  while (digitalRead(BUTTON) == LOW) {};
  leftServo.zero();
  rightServo.zero();
  delay(1000);
  while (digitalRead(BUTTON) == LOW) {};
  leftServo.reverseSlow();
  rightServo.reverseSlow();
  delay(1000);
  while (digitalRead(BUTTON) == LOW) {};
  leftServo.zero();
  rightServo.zero();
  delay(1000);
  while (digitalRead(BUTTON) == LOW) {};
  leftServo.reverseFast();
  rightServo.reverseFast();
  delay(1000);
  while (digitalRead(BUTTON) == LOW) {};
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
    side = gatedValueFilter(side, &previous, &gated, &count, 30, 5);
    Serial.print(150);
    Serial.print(",");
    Serial.println(side);
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
    // TODO: include roll conditions for one wheel on ramp
    if ((compass.a.y < -4000 && compass.a.y > -10000) || (compass.a.x < -3000 || compass.a.x > 3000)) {
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

void rotateToPost() {
#if RUN_MOTORS
  leftServo.slow();
  rightServo.reverseSlow();
#endif
  int delta = MAX_DISTANCE;
  int average = MAX_DISTANCE;
  int left = MAX_DISTANCE;
  int right = MAX_DISTANCE;

  int previous = MAX_DISTANCE;
  int count = 0;
  int gated = MAX_DISTANCE;
  bool lightOn = false;
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
    Serial.println(170);
#endif

    if (delta < 10) {
      if (average < 170 && !lightOn) {
        digitalWrite(RIGHT_LED, HIGH);
        lightOn = true;
        break;
      } else if (average > 170 && lightOn) {
        digitalWrite(RIGHT_LED, LOW);
        lightOn = false;
      }
    }

  }

#if RUN_MOTORS
  leftServo.zero();
  rightServo.zero();
#endif
}

void driveToPost() {
#if RUN_MOTORS
  leftServo.fast();
  rightServo.fast();
  delay(500); // TODO: this may remove acceleration from interfering with acc.
#endif
  while (true) {
    compass.read();
    // TODO: include roll conditions for one wheel on ramp
    if (compass.a.y < -4000 && compass.a.y > -10000) {
#if RUN_MOTORS
      delay(500);
      leftServo.zero();
      rightServo.zero();
#endif
      break;
    }
    if (compass.a.x < -3000 || compass.a.x > 3000) {
#if RUN_MOTORS
      delay(100);
      leftServo.zero();
      rightServo.zero();
#endif
    }
    delay(20);
  }
}

void findRamp() {
  leftServo.fast();
  rightServo.fast();
  delay(200);
  double irValue = MAX_DISTANCE;
  while (irValue > 50) {
    // TODO fix following
    irValue = irSensor.movingAvgIR(IRAvg, &total, &iteration, NUMBER_OF_READINGS);
#if DEBUG
    Serial.print(50);
    Serial.print(",");
    Serial.println(irValue);
#endif
    delay(10);
  }
  delay(225);
  leftServo.zero();
  rightServo.zero();
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
      delay(50);
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
  bool lightOn = false;
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
    Serial.println(15);
#endif
    if (average < 15) {
      lightOn = true;
      break;
    }
  }

#if RUN_MOTORS
  leftServo.zero();
  rightServo.zero();
#endif
}

void loop() {
    while (digitalRead(BUTTON) == LOW) {};

    delay(1000);
    findRamp();
    delay(500);
    shittyTurn90();
    delay(500);
    climbRamp();
    delay(500);
    driveTillCloseToWall();
    delay(500);
    shittyTurn902();
    delay(500);

    digitalWrite(RIGHT_LED, LOW);
    digitalWrite(LEFT_LED, LOW);
    if (driveTillPostOnLeft()) {
      // added delay before turn, for better centering
      leftServo.fast();
      rightServo.fast();
      delay(200);
      leftServo.zero();
      rightServo.zero();
      delay(500);
      shittyTurn902();
      delay(500);
//      if (!isPostInFront) {
//        rotateToPost();
//        delay(500);
//      }

      driveToPost();
    } else {
      leftServo.fast();
      rightServo.fast();
      delay(200);
      leftServo.zero();
      rightServo.zero();
    }
  //while (true) {};

}
