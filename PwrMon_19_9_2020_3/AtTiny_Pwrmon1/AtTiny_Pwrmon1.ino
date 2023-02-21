#include "EmonLib.h"                   // Include Energy monitor Library
#include "SoftwareSerial.h"

const int Rx = 3; // this is physical pin 2
const int Tx = 4; // this is physical pin 3
SoftwareSerial xSerial(Rx, Tx);

int pastmills = 0;
int pastminute = 0;

byte bcdToDec(byte val)  {
  // Convert binary coded decimal to normal decimal numbers
  return ( (val/16*10) + (val%16) );
}



const float minimumReading = 0.0;

//int mainDelay = 1000;   ///////////////MAIN DELAY TIME SETTING
float counterXX = 0;

int timesFactor = 1;

EnergyMonitor emon1, emon2;   
// Create 2 instances
 float Sval1;
 float Sval2;

 float ISval1;
 float ISval2;

 String dataString;

 String inputString = "";         // a String to hold incoming data





void setup() {//////////////////////////////////////////////SETUP////////////////////////////////////////////////////////

  pinMode(Rx, INPUT);
  pinMode(Tx, OUTPUT);

  xSerial.begin(115200);
  xSerial.println("<INIT>");
  emon1.current(0, 111.1);             // Current: input pin, calibration.
  emon2.current(1, 111.1);             // Set the correct input pin for channel 2, calibration.
  
  Sensor();   /////////// Inital Sensor run to clear invalid sensr values

 }




 void loop() {      /////////////////////////////////////LOOP////////////////////////////////////
  Sensor();
  
  xSerial.print (Sval1);
  xSerial.print (", ");
  xSerial.println (Sval2);


  ISval1 += Sval1;
  ISval2 += Sval2;



// counterXX = 0.00;
// ISval1 = 0.00;
// ISval2 = 0.00;


  //counterXX ++;

    while (xSerial.available()) {
    // get the new byte:
    char inChar = (char)xSerial.read();
    // add it to the inputString:
    inputString += inChar;

    if (inChar == '\n') {
      xSerial.println(inputString);
      
      if (inputString[0] == '<'){
          //TIME set string: <TIME>[2020/9/19 15:35:46]
//          if (inputString.substring(0,6) == "<TIME>"){ //RTC TIME SET INITIATED 
//            //Serial.println("Got Time");
//            deducetime(inputString.substring(6));
//            rtc.adjust(DateTime(outDate,outTime));
//            Serial.print("New RTC Time:");
//            Serial.print(outDate);
//            Serial.print(" ");
//            Serial.println(outTime);

        }
        else{  
        // not any of the above serial strings
        }
    }
      else{
        // not instance on '<'
      }
      inputString = "";
    }


}



void Sensor(){             ////////////////////////SENSOR/////////////////////////
 
  // Channel 1:
  double Irms = emon1.calcIrms(1480);  // Calculate Irms only
  // Serial.print(Irms5*230.0);        // Apparent power
  // Serial.print ("Sensor1: ");
  // Serial.print(Irms1/1.8);          // Irms
  Sval1= Irms/1.8;
  if(Sval1 <= minimumReading){
    Sval1 = 0;
  } 


  // Channel 2
  double Irms2 = emon2.calcIrms(1480);  // Calculate Irms only
  Sval2= Irms2/1.8;
  if(Sval2 <= minimumReading){
    Sval2 = 0;
  } 
 } 






