/*
	IRWayfarer.h header file
	
*/

#ifndef IRWayfarer_h
#define IRWayfarer_h

#include "Arduino.h"

class ServoWayfarer
{
	public:
	IRWayfarer(int pin);
        double linearlizeIR(); 
        double movingAvgIR(double IRavg[], double total, int iteration, int numReadings);
	
};

#endif
