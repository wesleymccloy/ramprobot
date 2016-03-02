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
    ServoWayfarer(int pin, int zeroAngle, bool direction, int slow, int fast, int reverseSlow, int reverseFast);
    void percentSpeed(int percent);
    void zero();
    void slow();
    void fast();
    void reverseSlow();
    void reverseFast();

  private:
    int _zeroAngle;
    bool _direction;
    int _slow;
    int _fast;
    int _reverseSlow;
    int _reverseFast;
};

#endif
