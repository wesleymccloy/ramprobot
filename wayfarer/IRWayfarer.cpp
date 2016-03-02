/*
	IRWayfarer.cpp
        Implementation of IRWayfarer

*/

#include "Arduino.h"
#include "IRWayfarer.h"

IRWayfarer::IRWayfarer(int pin) {
  _pin = pin;
}

double IRWayfarer::linearizeIR()
{ 
  int distance = analogRead(_pin); 

  return (2061.19) / pow(distance, 0.983284);
}

// take in the IRavg array, the current total, and the iteration number
double IRWayfarer::movingAvgIR(double IRavg[], double *total, int *iteration, int numReadings)
{
   *total = *total - IRavg[*iteration];
   IRavg[*iteration] = linearizeIR();
   if (IRavg[*iteration] > 500) { // In case you get an Inf
    IRavg[*iteration] = 500;
   }
   *total = *total + IRavg[*iteration];
   
   *iteration = *iteration + 1; 
   
   if (*iteration >= numReadings){
     *iteration = 0; 
   }
   
   double average; 

   
   average = *total/numReadings; 
   return average; 
   
}
