/*
BURDEN REISTOR: 18 Ω ±1% = Current Calibration Coefficient: 111.1 
BURDEN REISTOR: 22 Ω ±1% = Current Calibration Coefficient: 90.9 
BURDEN REISTOR: 120 Ω ±1% = Current Calibration Coefficient: 16.67 
BURDEN REISTOR: 33 Ω ±1% = Current Calibration Coefficient: 60.6 

CT Ratio / Burden resistance = (100A / 0.05A) / 33 Ohms = 60.6
***The SCT013 Current sensor is 100A:50mA output which = 2000 Turns***

*/
#include <EmonLib.h> // Include EmonLib Tiny Library: Local in folder
//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(0,1);  //rx, tx

EnergyMonitor emon1, emon2, emon3, emon4, emon5, emon6, emon7, emon8, emon9, emon10, emon11, emon12,  emon13, emon14; // Create an instance

void setup(){
  Serial.begin(9600);

  emon1.current(0, 60.6);             // Correct for SCT013 Current sensor.
  //emon1.current(0, 90.9);             // Current: input pin, calibration.
  //emon1.current(0, 111.1);             // Current: input pin, calibration.
  emon1.current(1, 60.6); 
  emon1.current(2, 60.6); 
  emon1.current(3, 60.6); 
  emon1.current(4, 60.6); 
  emon1.current(5, 60.6); 
  emon1.current(6, 60.6); 
  emon1.current(7, 60.6); 
  emon1.current(8, 60.6); 
  emon1.current(9, 60.6); 
  emon1.current(10, 60.6); 
  emon1.current(11, 60.6); 
  emon1.current(12, 60.6); 
  emon1.current(13, 60.6); 
}

void loop()
{
double Irms1 = emon1.calcIrms(500);  // Calculate Irms only
double Irms2 = emon2.calcIrms(500);  // Calculate Irms only
double Irms3 = emon3.calcIrms(500);  // Calculate Irms only
double Irms4 = emon4.calcIrms(500);  // Calculate Irms only
double Irms5 = emon5.calcIrms(500);  // Calculate Irms only
double Irms6 = emon6.calcIrms(500);  // Calculate Irms only
double Irms7 = emon7.calcIrms(500);  // Calculate Irms only
double Irms8 = emon8.calcIrms(500);  // Calculate Irms only
double Irms9 = emon9.calcIrms(500);  // Calculate Irms only
double Irms10 = emon10.calcIrms(500);  // Calculate Irms only
double Irms11 = emon11.calcIrms(500);  // Calculate Irms only
double Irms12 = emon12.calcIrms(500);  // Calculate Irms only
double Irms13 = emon13.calcIrms(500);  // Calculate Irms only
double Irms14 = emon14.calcIrms(500);  // Calculate Irms only


  Serial.print("S1: ");
  Serial.print(Irms1*240.0);           // Apparent power
  Serial.print(" ");
  Serial.print(Irms1);             // Irms
  Serial.print(", ");

  Serial.print("S2: ");
  Serial.print(Irms2*240.0);           // Apparent power
  Serial.print(" ");
  Serial.print(Irms2);             // Irms
  Serial.print(", ");

  Serial.print("S3: ");
  Serial.print(Irms3*240.0);           // Apparent power
  Serial.print(" ");
  Serial.print(Irms3);             // Irms
  Serial.print(", ");

   Serial.print("S4: ");
  Serial.print(Irms4*240.0);           // Apparent power
  Serial.print(" ");
  Serial.print(Irms4);             // Irms
  Serial.print(", ");

    Serial.print("S5: ");
  Serial.print(Irms5*240.0);           // Apparent power
  Serial.print(" ");
  Serial.print(Irms5);             // Irms
  Serial.print(", ");

    Serial.print("S6: ");
  Serial.print(Irms6*240.0);           // Apparent power
  Serial.print(" ");
  Serial.print(Irms6);             // Irms
  Serial.print(", ");

    Serial.print("S7: ");
  Serial.print(Irms7*240.0);           // Apparent power
  Serial.print(" ");
  Serial.print(Irms7);             // Irms
  Serial.print(", ");

    Serial.print("S8: ");
  Serial.print(Irms8*240.0);           // Apparent power
  Serial.print(" ");
  Serial.print(Irms8);             // Irms
  Serial.print(", ");

    Serial.print("S9: ");
  Serial.print(Irms9*240.0);           // Apparent power
  Serial.print(" ");
  Serial.print(Irms9);             // Irms
  Serial.print(", ");

    Serial.print("S10: ");
  Serial.print(Irms10*240.0);           // Apparent power
  Serial.print(" ");
  Serial.print(Irms10);             // Irms
  Serial.print(", ");

    Serial.print("S11: ");
  Serial.print(Irms11*240.0);           // Apparent power
  Serial.print(" ");
  Serial.print(Irms11);             // Irms
  Serial.print(", ");

    Serial.print("S12: ");
  Serial.print(Irms12*240.0);           // Apparent power
  Serial.print(" ");
  Serial.print(Irms12);             // Irms
  Serial.print(", ");

    Serial.print("S13: ");
  Serial.print(Irms13*240.0);           // Apparent power
  Serial.print(" ");
  Serial.print(Irms13);             // Irms
  Serial.print(", ");

    Serial.print("S14: ");
  Serial.print(Irms14*240.0);           // Apparent power
  Serial.print(" ");
  Serial.print(Irms14);             // Irms
  Serial.println("");



  
}

