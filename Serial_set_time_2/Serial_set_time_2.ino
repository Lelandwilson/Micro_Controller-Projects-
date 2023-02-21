// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

#include <Wire.h>
#include "RTClib.h"

DS1307 rtc;

String inputString = "";         // a String to hold incoming data

String tester = "[2020/01/01 01:01:01]";
String yearX = "";
String monthX= "";
String dayX= "";
String hourX= "";
String minuteX= "";
String secondX= "";

char outDate[12] = "";
char outTime[9] = "";
  
void setup () {


  Serial.begin(9600);
  Serial.println("<INIT>");
  
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  
  #ifdef AVR
    Wire.begin();
  #else 
    Wire1.begin();
  #endif
    rtc.begin();

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
  }

//[2020/9/18 18:38:46]
//  Serial.print("Sample<");
//  Serial.print(tester);
//  Serial.println(">");

  
  // [2020/9/18 18:38:46]
  // sample input: date = "Dec 26 2009", time = "12:34:56"
  //rtc.adjust(DateTime("Dec 26 2009", "12:34:56"));
  deducetime(tester);
  rtc.adjust(DateTime(outDate,outTime));
  delay(100);

}

void loop () {
    DateTime now = rtc.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

      // print the string when a newline arrives:
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;

    if (inChar == '\n') {
      Serial.println(inputString);
      if (inputString[0] == '<'){
        //<TIME>[2020/9/19 16:06:00]//
        Serial.println(inputString.substring(0,6));
        if (inputString.substring(0,6) == "<TIME> Function intiated");
        //Serial.println("<OK>");
        
        deducetime(inputString.substring(6));
        rtc.adjust(DateTime(outDate,outTime));
        
        Serial.print("New RTC Time:");
        Serial.print(outDate);
        Serial.print(" ");
        Serial.println(outTime);
        
      }

      //Serial.println(inputString[0]);
      
      inputString = "";
    }
  }


    delay(3000);
}

void deducetime(String x){

  int openBracket = x.indexOf('[');
  int firstSlash = x.indexOf('/');
  int secondSlash = x.lastIndexOf('/');
  int firstSpace = x.indexOf(' ');
  int firstColon = x.indexOf(':');
  int secondColon = x.lastIndexOf(':');
  int closeBracket = x.indexOf(']');
  
  String yearX = "";
  String monthX = "";
  String dayX = "";
  String hourX = "";
  String minuteX = "";
  String secondX = "";

  int yearY = "";
  int monthY = "";
  int dayY = "";
  int hourY = "";
  int minuteY = "";
  int secondY = "";

  String shortMonth = "";
  
  
  yearX = x.substring(openBracket+1, firstSlash);
  monthX = x.substring(firstSlash+1, secondSlash);
  dayX = x.substring(secondSlash+1, firstSpace);
  hourX = x.substring(firstSpace+1, firstColon);
  minuteX = x.substring(firstColon+1, secondColon);
  secondX = x.substring(secondColon+1, closeBracket);

  yearY = yearX.toInt(); 
  monthY = monthX.toInt();
  dayY = dayX.toInt();
  hourY = hourX.toInt();
  minuteY = minuteX.toInt();
  secondY = secondX.toInt();

  if (monthY == 1){
    shortMonth = "Jan";
  }
  else if (monthY == 2){
    shortMonth = "Feb";
  }
  else if (monthY == 3){
  shortMonth = "Mar";
  }
  else if (monthY == 4){
  shortMonth = "Apr";
  }
  else if (monthY == 5){
  shortMonth = "May";
  }
  else if (monthY == 6){
  shortMonth = "Jun";
  }
  else if (monthY == 7){
  shortMonth = "Jul";
  }
  else if (monthY == 8){
  shortMonth = "Aug";
  }
  else if (monthY == 9){
  shortMonth = "Sep";
  }
  else if (monthY == 10){
  shortMonth = "Oct";
  }
  else if (monthY == 11){
  shortMonth = "Nov";
  }
  else{
  shortMonth = "Dec";
  }

  if (dayX.length()<2){
    dayX = "0"+ dayX;
    }

  if (minuteX.length()<2){
    minuteX = "0"+ minuteX;
    }

  if (secondX.length()<2){
    secondX = "0"+ secondX;
    }

  
  String Date = shortMonth+" "+dayX+" "+yearX;
  String Time = hourX+":"+minuteX+":"+secondX;

  Date.toCharArray(outDate, 12);
  Time.toCharArray(outTime, 9);

  
  return(outDate,outTime);

}


