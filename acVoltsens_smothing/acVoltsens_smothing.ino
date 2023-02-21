
#include "EmonLib.h"             // Include Emon Library
EnergyMonitor emon1;             // Create an instance


const int numReadings = 10;

float readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
float total = 0;                  // the running total
float average = 0;                // the average



void setup()
{  
  Serial.begin(9600);
  emon1.voltage(A2, 234.26, 1.7);  // Voltage: input pin, calibration, phase_shift
  //emon1.current(1, 111.1);       // Current: input pin, calibration.

    // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}
void loop()
{
  emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
  //emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
float realPower       = emon1.realPower;        //extract Real Power into variable
float apparentPower   = emon1.apparentPower;    //extract Apparent Power into variable
float powerFActor     = emon1.powerFactor;      //extract Power Factor into Variable
float supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
float Irms            = emon1.Irms;             //extract Irms into Variable

//-----------------------------//
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  float Vin;
  Vin = analogRead(A2);
  Vin = ((Vin/1023)*5);
  readings[readIndex] = Vin;
  
//  readings[readIndex] = analogRead(A2);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  // send it to the computer as ASCII digits
  //Serial.println(average);




Serial.print("Supply: ");
Serial.print(emon1.Vrms);
Serial.print(", Input:");

//float Vin;
//Vin = ((average/1023)*5);
Serial.println(average);
}
