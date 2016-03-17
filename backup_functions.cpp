// backup functions

// Does not work well
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

// Does not work well
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


// Keep for testing
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


// Keep for testing
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


// Keep for testing
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
  }
}

// Not using function at the moment
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
