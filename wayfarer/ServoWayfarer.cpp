/*
	ServoWayfarer.cpp
	implementation of ServoWayfarer class

*/

#include "Arduino.h"
#include <Servo.h> // not necessary I don't think
#include "ServoWayfarer.h"

ServoWayfarer::ServoWayfarer(int zeroAngle, bool direction, int slow, int fast, int reverseSlow, int reverseFast, int slowRotate, int slowRamp) : Servo() {
  _direction = direction;
  _zeroAngle = zeroAngle;
  if (!_direction) {
    _slow = -slow;
    _fast = -fast;
    _reverseSlow = -reverseSlow;
    _reverseFast = -reverseFast;
    _slowRotate = -slowRotate;
    _slowRamp = -slowRamp;
  } else {
    _slow = slow;
    _fast = fast;
    _reverseSlow = reverseSlow;
    _reverseFast = reverseFast;
    _slowRotate = slowRotate;
    _slowRamp = slowRamp;
  }
}

void ServoWayfarer::zero() {
  Servo::write(_zeroAngle);
  return;
}
void ServoWayfarer::slow() {
  Servo::write(_zeroAngle + _slow);
  return;
}
void ServoWayfarer::fast() {
  Servo::write(_zeroAngle + _fast);
  return;
}
void ServoWayfarer::reverseSlow() {
  Servo::write(_zeroAngle + _reverseSlow);
  return;
}
void ServoWayfarer::reverseFast() {
  Servo::write(_zeroAngle + _reverseFast);
  return;
}
void ServoWayfarer::slowRotate() {
  Servo::write(_zeroAngle + _slowRotate);
  return;
}
void ServoWayfarer::slowRamp() {
  Servo::write(_zeroAngle + _slowRamp);
  return;
}

void ServoWayfarer::percentSpeed(int percent) {
  int angleOffset = 0;
  if (!_direction) {
    percent = percent * (-1);
  }
  if (percent == 0) {
    Servo::write(_zeroAngle);
    return;
  } else if (percent > 0 && percent <= 100) {
    angleOffset = floor((double)(percent) / 100.0 * ((180.0 - _zeroAngle)));
  } else if (percent < 0 && percent >= -100) {
    angleOffset = floor((double)(percent) / 100.0 * ((double)(_zeroAngle)));
  }
  Servo::write(_zeroAngle + angleOffset);
  return;
}
