/*
	IRWayfarer.cpp
        Implementation of IRWayfarer

*/

#include "Arduino.h"
#include "IRWayfarer.h"

IRWayfarer::IRWayfarer(int pin) {

}

double IRWayfarer::linearizeIR
{
  int distance; 
  distance = analogRead(pin); 
  return (2061.19) / pow(distance, 0.983284);
}

// take in the IRavg array, the current total, and the iteration number
double IRWayfarer::movingAvgIR(double IRavg[], double total, int iteration, int numReadings)
{
   total = total - IRavg[iteration];
   IRavg[iteration] = linearizeIR; 
   total = total + IRavg[iteration];
   
   iteration ++; 
   
   if (iteration >= numReadings){
     iteration = 0; 
   }
   
   double average; 
   
   average = total/numReadings; 
   return average; 
   
}
