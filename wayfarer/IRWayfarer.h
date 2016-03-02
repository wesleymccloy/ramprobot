/*
	IRWayfarer.h header file
	
*/

#ifndef IRWayfarer_h
#define IRWayfarer_h

#include "Arduino.h"

class IRWayfarer
{
	public:
	IRWayfarer(int pin);
  double linearizeIR(); 
  double movingAvgIR(double IRavg[], double *total, int *iteration, int numReadings);
	private:
  int _pin;
};

#endif
