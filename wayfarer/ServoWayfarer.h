/*
	ServoWayfarer.h header file
	This custom subclass of Servo to be able to do percentages instead of
	angles
*/

#ifndef ServoWayfarer_h
#define ServoWayfarer_h

#include "Arduino.h"
#include <Servo.h>

class ServoWayfarer : public Servo
{
	public:
	ServoWayfarer(int pin, int zeroAngle, bool direction);
	void percentSpeed(int percent);

	private:
	int _zeroAngle;
	bool _direction;
};

#endif
