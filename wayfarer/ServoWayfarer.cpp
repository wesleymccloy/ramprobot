/*
	ServoWayfarer.cpp
	implementation of ServoWayfarer class

*/

#include "Arduino.h"
#include <Servo.h> // not necessary I don't think
#include "ServoWayfarer.h"

ServoWayfarer::ServoWayfarer(int pin, int zeroAngle, bool direction) : Servo() {
	Servo::attach(pin);
	_direction = direction;
	_zeroAngle = zeroAngle;
}

void ServoWayfarer::percentSpeed(int percent) {
	int angleOffset = 0;
	if (!_direction) {
		percent = percent*(-1);
	}
	if (percent == 0) {
		Servo::write(_zeroAngle);
	} else if (percent > 0 && percent <= 100) {
		int angleOffset = floor((double)(percent)/100.0*(180.0-_zeroAngle));
	} else if (percent < 0 && percent >= -100) {
		int angleOffset = floor((double)(percent)/100.0*(double)(_zeroAngle));
	}
	Servo::write(_zeroAngle+angleOffset);
}
