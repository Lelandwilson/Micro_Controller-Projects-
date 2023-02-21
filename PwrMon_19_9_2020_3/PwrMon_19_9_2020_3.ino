#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include "EmonLib.h"                   // Include Energy monitor Library
#include "Wire.h"                   // For RTC
#include <EEPROM.h>                   // For saving data to eeprom memory

//#include <arduino-timer.h>            //timer/ interrupt outside of loop to remove delay 
//auto timer = timer_create_default(); // create a timer with default settings

int pastmills = 0;
int pastminute = 0;
String Version_X = "V:1.02";     ///////////VERSION//////////

//For seting RTC time
DS1307 rtc;
#define DS1307_ADDRESS 0x68// RTC I2C Address
byte bcdToDec(byte val)  {
  // Convert binary coded decimal to normal decimal numbers
  return ( (val/16*10) + (val%16) );
}


int addr = 0; // EEPROM address
//int Unitnumber = 0;
int saveSDdata_interval = 60; //Save to SD card every (X *1) seconds
//EEPROM.write(addr, Unitnumber); ///////////Set UNit Number //////////
//postWriteEEPROMmove();
int Unitnumber = EEPROM.read(0); /////////// UNIT Number /////////////

String DATA_FILE = "datalog.csv";


///////PIN ASSIGNMENTS///////////
int cs =  53; // 'CS' pin on TFT
int dc =  47;  // 'A0' or 'RS' pin on TFT
int rst = 48;  //  'RESET' pin on TFT -You can also connect this to the Arduino reset

//  Below 2 are set by default // 51 & 52 are hardware SPI and are fastest option
//int LCDsck = 52 // 'SCK' on TFT
//int LCDmosi = 51 // 'SDA" or 'MOSI' on TFT

Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);

//int LcdReset = 34;   //alternative LCD (not in use)
const int lcd2 = 49;    // 'LED'- LCD Backlight Pin on TFT
const int chipSelect = 46;   //SD CARD SELECT      /////////PIN 4 for ETHERCARD
const int but1= 5;
const int but2 = 6;
const int switch1 = 7;

const float minimumReading = 0.3;
int but1var = 1;
int but2var = 1;
int switch1var = 1;
int ledState = HIGH;        
int but1State = 0; 
int but2State = 0;
int switch1state = 0;  
//int mainDelay = 1000;   ///////////////MAIN DELAY TIME SETTING
float counterXX = 0;

int timesFactor = 1;
char units = 'A';

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

#define   FILL1 tft.fillRect(1, 1, 124, 8, ST7735_BLACK)  //TIME BLANK
#define   FILL2 tft.fillRect(1, 13, 124, 143, ST7735_BLACK)  //BODY BLANK
#define   FILL3 tft.fillRect(60, 13, 65, 143, ST7735_BLACK)  //BODY BLANK
#define   SQUARE0 tft.drawRect(0, 20, 126, 45, ST7735_WHITE) //INITIAL WHITE SQUARE
#define   SQUARE1 tft.drawRect(0, 10, 126, 148, ST7735_WHITE) //BODY WHITE SQUARE
#define   SQUARE2 tft.drawRect(0, 0, 126, 11, ST7735_WHITE) //TIME WHITE SQUARE


File dataFile;
EnergyMonitor emon1, emon2, emon3, emon4, emon5, emon6, emon7, emon8, emon9, emon10, emon11, emon12, emon13, emon14;   
// Create 14 instances
 float Sval1;
 float Sval2;
 float Sval3;
 float Sval4;
 float Sval5;
 float Sval6;
 float Sval7;
 float Sval8;
 float Sval9;
 float Sval10;
 float Sval11;
 float Sval12;
 float Sval13;
 float Sval14;


 float ISval1;
 float ISval2;
 float ISval3;
 float ISval4;
 float ISval5;
 float ISval6;
 float ISval7;
 float ISval8;
 float ISval9;
 float ISval10;
 float ISval11;
 float ISval12;
 float ISval13;
 float ISval14;

 int second; 
 int minute;
 int hour;
 int weekDay; 
 int monthDay; 
 int month; 
 int year; 
 //int Vpin= 7;
 String dataString;

 String inputString = "";         // a String to hold incoming data

////////////////// Set RTC time via Serial variables ////////////
String tester = "[2020/01/01 01:01:01]";
String yearX = "";
String monthX= "";
String dayX= "";
String hourX= "";
String minuteX= "";
String secondX= "";

char outDate[12] = "";
char outTime[9] = "";

 



 void setup() {//////////////////////////////////////////////SETUP////////////////////////////////////////////////////////

//   TCCR1A = 0;
//   TCCR1B = 0;
//   TCNT1 = 3036;
//   TCCR1B |= (1<<CS12); //prescaler 1
//   TIMSK1 |= (1<<TOIE1); //enable timer overflow


 // call the toggle_timer function every 1000 millis (1 second)
  //timer.every(1000, toggle_timer);

  // Initilize RTC  //
  #ifdef AVR
  Wire.begin();
  #else
    Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
  #endif
  rtc.begin();

  // pinMode(Power, OUTPUT);
  //pinMode(LcdReset, OUTPUT); //alternative LCD (not in use)
  pinMode(lcd2, OUTPUT);    //RS pin For TFT
  pinMode(but1, INPUT);
  pinMode(but2, INPUT);
  pinMode(switch1, INPUT);

  Serial.begin(115200);
  
  Sensor();   /////////// Inital Sensor run to clear invalid sensr values '45'

  //SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0)); //Can artificially set SPI clock speed


  tft.initR(INITR_BLACKTAB);   // initialize a ST7735R chip, red tab
  tft.setRotation(2);        //TFT Screen Orientation
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.invertDisplay(false);
  delay(30);
 // lcd2, LOW;  //Back Light PIN ON TFT                           ///////For other LCD's
  digitalWrite(lcd2, HIGH);                       ///////USE THIS ONE FOR SMALL RED SCREENS
  delay(10);

  ////////////////////////////////////LCD1 message///////////////////////////////

  SQUARE0;

  Serial.print("Initializing SD card...");
  pinMode(SS, OUTPUT);

  tft.setCursor(0, 30);
  tft.fillScreen(ST7735_BLACK);
  SQUARE0;
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.println("     INITILIZING");
  tft.setCursor(0, 45);
  tft.println("       SD CARD");
  delay(500);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    
    tft.setCursor(0, 30);
    tft.fillScreen(ST7735_BLACK);
    SQUARE0;
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("         CARD ");
    tft.setCursor(0, 45);
    tft.println("         FAIL");

    // don't do anything more:
    //while (1) ;
  }
  
  Serial.println("Card initialized.");
  tft.setCursor(0, 30);
  tft.fillScreen(ST7735_BLACK);
  SQUARE0;
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.println("         CARD ");
  tft.setCursor(0, 45);
  tft.println("     INITIALIZED");
  delay(500);


  // Open up the file we're going to log to!
  dataFile = SD.open(DATA_FILE, FILE_WRITE);
  if (! dataFile) {
    Serial.println("Error opening datalog.csv");

    tft.setCursor(0, 30);
    tft.fillScreen(ST7735_BLACK);
    SQUARE0;
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("      Failed to ");
    tft.setCursor(0, 45);
    tft.println("       open file");

    delay(2000); 
    SQUARE1;
    SQUARE2;
    FILL2;
  }

  emon1.current(0, 111.1);             // Current: input pin, calibration.
  emon2.current(1, 111.1);             // Set the correct input pin for channel 2, calibration.
  emon3.current(2, 111.1);
  emon4.current(3, 111.1);
  emon5.current(4, 111.1);
  emon6.current(5, 111.1);
  emon7.current(6, 111.1);
  emon8.current(7, 111.1);
  emon9.current(8, 111.1);
  emon10.current(9, 111.1); 
  emon11.current(10, 111.1);
  emon12.current(11, 111.1);
  emon13.current(12, 111.1);
  emon14.current(13, 111.1);
  
 
  SQUARE1;
  SQUARE2;
  FILL2;
 
  Sensor();
  delay(2000);

 }




 void loop() {      /////////////////////////////////////LOOP////////////////////////////////////
  
  //timer.tick();
  serialInput();
  button1();
  button2();
  toggleswitch1();
  runmain();
  //Sensor();
  


}

void runmain(){
  Sensor();

  Wire.beginTransmission(DS1307_ADDRESS);

  byte zero = 0x00;
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);

  int second = bcdToDec(Wire.read());
  int minute = bcdToDec(Wire.read());
  int hour = bcdToDec(Wire.read() & 0b111111); //24 hour time
  int weekDay = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
  int monthDay = bcdToDec(Wire.read());
  int month = bcdToDec(Wire.read());
  int year = bcdToDec(Wire.read());

  Serial.print("U");
  Serial.print(Unitnumber);
  Serial.print(",");
  Serial.print (monthDay);
  Serial.print ("/");
  Serial.print (month);
  Serial.print (" ");
  Serial.print (hour);
  Serial.print (":");
  Serial.print (minute);
  Serial.print (", ");

  FILL1;

  tft.setCursor(0,2);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.print(" U");
  tft.print(Unitnumber);
  tft.print(" | ");
  tft.print(monthDay);
  tft.print("/");
  tft.print(month);
  tft.print(" ");
  tft.print(hour);
  tft.print(":");
  tft.print(minute);
 
  Serial.print (Sval1);
  Serial.print (", ");
  Serial.print (Sval2);
  Serial.print (", ");
  Serial.print (Sval3);
  Serial.print (", ");
  Serial.print (Sval4);
  Serial.print (", ");
  Serial.print (Sval5);
  Serial.print (", ");
  Serial.print (Sval6);
  Serial.print (", ");
  Serial.print (Sval7);
  Serial.print (", ");
  Serial.print (Sval8);
  Serial.print (", ");
  Serial.print (Sval9);
  Serial.print (", ");
  Serial.print (Sval10);
  Serial.print (", ");
  Serial.print (Sval11);
  Serial.print (", ");
  Serial.print (Sval12);
  Serial.print (", ");
  Serial.print (Sval13);
  Serial.print (", ");
  Serial.println (Sval14);

  ISval1 += Sval1;
  ISval2 += Sval2;
  ISval3 += Sval3;
  ISval4 += Sval4;
  ISval5 += Sval5;
  ISval6 += Sval6;
  ISval7 += Sval7;
  ISval8 += Sval8;
  ISval9 += Sval9;
  ISval10 += Sval10;
  ISval11 += Sval11;
  ISval12 += Sval12;
  ISval13 += Sval13;
  ISval14 += Sval14;

//  if (minute != pastminute){ //Save every 1 minute
//      Serial.println("<SAVE>");
//  }

  dataFile = SD.open(DATA_FILE, FILE_WRITE);
  if (! dataFile) {
  // 
  //    tft.setCursor(0, 30);
  //    tft.fillScreen(ST7735_BLACK);
  //    FILL2;
  //    SQUARE0;
  //    tft.setTextColor(ST7735_WHITE);
  //    tft.setTextSize(1);
  //    tft.println("    SD card error ");
  //    tft.println("   Re-insert card");
      // don't do anything more:
      //while (1) ;
  ////  delay(10000);  // Allow time for card to be inserted
  //  SQUARE1;
  //  SQUARE2;
  //  FILL2;
  }
  else{
    if (minute != pastminute){ //Save every 1 minute
      //Serial.println("<SAVE>");
      dataFile.print(monthDay);
      dataFile.print("/");  
      
      dataFile.print(month);
      dataFile.print(" ");  
  
      dataFile.print(hour);
      dataFile.print(":"); 
  
      dataFile.print(minute);
      dataFile.print(", ");  
      
      dataFile.print(ISval1/counterXX);
      dataFile.print(", ");
      
      dataFile.print(ISval2/counterXX);
      dataFile.print(", ");
    
      dataFile.print(ISval3/counterXX);
      dataFile.print(", ");
     
      dataFile.print(ISval4/counterXX);
      dataFile.print(", ");
    
      dataFile.print(ISval5/counterXX);
      dataFile.print(", ");
     
      dataFile.print(ISval6/counterXX);
      dataFile.print(", ");
    
      dataFile.print(ISval7/counterXX);
      dataFile.print(", ");
    
      dataFile.print(ISval8/counterXX);
      dataFile.print(", ");
    
      dataFile.print(ISval9/counterXX);
      dataFile.print(", ");
    
      dataFile.print(ISval10/counterXX);
      dataFile.print(", ");
    
      dataFile.print(ISval11/counterXX);
      dataFile.print(", ");
    
      dataFile.print(ISval12/counterXX);
      dataFile.print(", ");
    
      dataFile.print(ISval13/counterXX);
      dataFile.print(", ");
    
      dataFile.println(ISval14/counterXX);
      dataFile.close();
  
      counterXX = 0.00;
      ISval1 = 0.00;
      ISval2 = 0.00;
      ISval3 = 0.00;
      ISval4 = 0.00;
      ISval5 = 0.00;
      ISval6 = 0.00;
      ISval7 = 0.00;
      ISval8 = 0.00;
      ISval9 = 0.00;
      ISval10 = 0.00;
      ISval11 = 0.00;
      ISval12 = 0.00;
      ISval13 = 0.00;
      ISval14 = 0.00;
 
      }
   }

  
  FILL3;
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);

  tft.setCursor(2,14);
  tft.print("Sensor 1:  ");
  tft.print((Sval1*timesFactor));
  tft.print(" ");
  tft.println((units));

  tft.setCursor(2,24);
  tft.print("Sensor 2:  ");
  tft.print((Sval2*timesFactor));
  tft.print(" ");
  tft.println((units));

  tft.setCursor(2,34);
  tft.print("Sensor 3:  ");
  tft.print((Sval3*timesFactor));
  tft.print(" ");
  tft.println((units));

  tft.setCursor(2,44);
  tft.print("Sensor 4:  ");
  tft.print((Sval4*timesFactor));
  tft.print(" ");
  tft.println((units));

  tft.setCursor(2,54);
  tft.print("Sensor 5:  ");
  tft.print((Sval5*timesFactor));
  tft.print(" ");
  tft.println((units));

  tft.setCursor(2,64);
  tft.print("Sensor 6:  ");
  tft.print((Sval6*timesFactor));
  tft.print(" ");
  tft.println((units));

  tft.setCursor(2,74);
  tft.print("Sensor 7:  ");
  tft.print((Sval7*timesFactor));
  tft.print(" ");
  tft.println((units));

  tft.setCursor(2,84);
  tft.print("Sensor 8:  ");
  tft.print((Sval8*timesFactor));
  tft.print(" ");
  tft.println((units));

  tft.setCursor(2,94);
  tft.print("Sensor 9:  ");
  tft.print((Sval9*timesFactor));
  tft.print(" ");
  tft.println((units));

  tft.setCursor(2,104);
  tft.print("Sensor 10: ");
  tft.print((Sval10*timesFactor));
  tft.print(" ");
  tft.println((units));

  tft.setCursor(2,114);
  tft.print("Sensor 11: ");
  tft.print((Sval11*timesFactor));
  tft.print(" ");
  tft.println((units));

  tft.setCursor(2,124);
  tft.print("Sensor 12: ");
  tft.print((Sval12*timesFactor)); 
  tft.print(" ");
  tft.println((units)); 

  tft.setCursor(2,134);
  tft.print("Sensor 13: ");
  tft.print((Sval13*timesFactor));
  tft.print(" ");
  tft.println((units));
  
  tft.setCursor(2,144);
  tft.print("Sensor 14: ");
  tft.print((Sval14*timesFactor));
  tft.print(" ");
  tft.println((units));

  counterXX ++;
  pastminute = minute;
  //TCNT1 = 3036;
  //Serial.println(millis()-pastmills);
  //pastmills = millis();

  
  //delay(mainDelay);      /////////////////////Main Delay///////////////////////

  //return true; // repeat? true
}

void serialInput(){
        // print the string when a newline arrives:
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;

    if (inChar == '\n') {
      Serial.print(inputString);
      
      if (inputString[0] == '<'){
          //TIME set string: <TIME>[2020/9/19 15:35:46]
          if (inputString.substring(0,6) == "<TIME>"){ //RTC TIME SET INITIATED 
            //Serial.println("Got Time");
            deducetime(inputString.substring(6));
            rtc.adjust(DateTime(outDate,outTime));
            Serial.print("New RTC Time:");
            Serial.print(outDate);
            Serial.print(" ");
            Serial.println(outTime);
          }
          //ID set string <UNID_ID>[1]
          else if (inputString.substring(0,9) == "<UNID_ID>"){ //UNIT ID SET INITIATED 
            Serial.println("New UNIT_ID");
            String input = inputString.substring(10);
            int openBracket = input.indexOf('[');
            int closeBracket = input.indexOf(']');
            int UnitNo = input.substring(openBracket,closeBracket-1).toInt();
            Unitnumber = UnitNo;
            //Serial.println(UnitNo);
            //Serial.println("<Writing to EEPROM>");
            EEPROM.write(addr, UnitNo); ///////////Set UNit Number //////////
            postWriteEEPROMmove();
          }
          else if (inputString.substring(0,9) == "<LCD_OFF>"){ //LCD OFF 
            Serial.println("LCD now OFF");
            digitalWrite(lcd2, LOW);
          }
          else if (inputString.substring(0,8) == "<LCD_ON>"){ //LCD ON 
            Serial.println("LCD now ON");
            digitalWrite(lcd2, HIGH);
          }
          else if (inputString.substring(0,9) == "<VERSION>"){ //Firmware Version 
            Serial.print("Firmware: ");
            Serial.println(Version_X);
          }
          else if (inputString.substring(0,12) == "<DEFAULT_SD>"){ //Firmware Version 
            Serial.println("ERASING ALL DATA ON SD CARD");
            SD.remove(DATA_FILE);
            if (SD.exists(DATA_FILE)){ 
              Serial.print("Error: Could not locate files: ");
              Serial.println(DATA_FILE);
             }
             else{
              Serial.println("SD card default sucessful");
             }
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


void printDate(){            /////////////////////RTC TIMER////////////////////////

  // Reset the register pointer
  Wire.beginTransmission(DS1307_ADDRESS);

  byte zero = 0x00;
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);

  int second = bcdToDec(Wire.read());
  int minute = bcdToDec(Wire.read());
  int hour = bcdToDec(Wire.read() & 0b111111); //24 hour time
  int weekDay = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
  int monthDay = bcdToDec(Wire.read());
  int month = bcdToDec(Wire.read());
  int year = bcdToDec(Wire.read());

  Serial.print("U");
  Serial.print(Unitnumber);
  Serial.print(",");
  
  Serial.print (monthDay);
  Serial.print ("/");

  Serial.print (month);
  Serial.print (" ");

  Serial.print (hour);
  Serial.print (":");

  Serial.print (minute);
  Serial.print (", ");

//  dataFile = SD.open(DATA_FILE, FILE_WRITE);
//  if (! dataFile) {
//    
//  }
//  else{
  
    if (counterXX >= saveSDdata_interval){
      dataFile.print(monthDay);
      dataFile.print("/");  
      
      dataFile.print(month);
      dataFile.print(" ");  
  
      dataFile.print(hour);
      dataFile.print(":"); 
  
      dataFile.print(minute);
      dataFile.print(", ");
    }
  //}

  FILL1;

  tft.setCursor(0,2);

  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.print(" U");
  tft.print(Unitnumber);
  tft.print(" | ");
  tft.print(monthDay);
  tft.print("/");
  tft.print(month);
  tft.print(" ");
  tft.print(hour);
  tft.print(":");
  tft.print(minute);

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


  // Channel 3
  double Irms3 = emon3.calcIrms(1480);  // Calculate Irms only
  Sval3= Irms3/1.8;
  if(Sval3 <= minimumReading){
    Sval3 = 0;  
  } 


  // Channel 4
  double Irms4 = emon4.calcIrms(1480);  // Calculate Irms only
  Sval4= Irms4/1.8;
  if(Sval4 <= minimumReading){
    Sval4 = 0;  
  } 

  // Channel 5   
  double Irms5 = emon5.calcIrms(1480);  // Calculate Irms only
  Sval5= Irms5/1.8;
  if(Sval5 <= minimumReading){
    Sval5 = 0;  
  } 

  // Channel 6   
  double Irms6 = emon6.calcIrms(1480);  // Calculate Irms only
  Sval6= Irms6/1.8;
  if(Sval6 <= minimumReading){
    Sval6 = 0;
  } 

  // Channel 7   
  double Irms7 = emon7.calcIrms(1480);  // Calculate Irms only
  Sval7= Irms7/1.8;
  if(Sval7 <= minimumReading){
    Sval7 = 0;
  } 

  // Channel 8   
  double Irms8 = emon8.calcIrms(1480);  // Calculate Irms only
  Sval8= Irms8/1.8;
  if(Sval8 <= minimumReading){
    Sval8 = 0;  
  } 

  // Channel 9   
  double Irms9 = emon9.calcIrms(1480);  // Calculate Irms only
  Sval9= Irms9/1.8;
  if(Sval9 <= minimumReading){
    Sval9 = 0;
  } 

  // Channel 10   
  double Irms10 = emon10.calcIrms(1480);  // Calculate Irms only
  Sval10= Irms10/1.8; 
  if(Sval10 <= minimumReading){
    Sval10 = 0;
  } 

  // Channel 11   
  double Irms11 = emon11.calcIrms(1480);  // Calculate Irms only
  Sval11= Irms11/1.8;
 if(Sval11 <= minimumReading){
    Sval11 = 0;
  }  

  // Channel 12   
  double Irms12 = emon12.calcIrms(1480);  // Calculate Irms only
  Sval12= Irms12/1.8;  
  if(Sval12 <= minimumReading){
    Sval12 = 0;
  } 

  // Channel 13   
  double Irms13 = emon13.calcIrms(1480);  // Calculate Irms only
  Sval13= Irms13/1.8;
  if(Sval13 <= minimumReading){
    Sval13 = 0;
  }   
  
    // Channel 14   
  double Irms14 = emon14.calcIrms(1480);  // Calculate Irms only
  Sval14= Irms14/1.8;
  if(Sval14 <= minimumReading){
    Sval14 = 0;
  }  


 }


void button1(){             ////////////////////////BUTTON 1/////////////////////////
  but1State = digitalRead(but1);
  ledState = digitalRead(lcd2);
  if (but1State == HIGH) {
      //Serial.println("Button 1 press"); 
      if (ledState == HIGH){
         digitalWrite(lcd2, LOW);
      }
      else{
        digitalWrite(lcd2, HIGH);
      }
        
  }
}


void button2(){              ////////////////////////BUTTON 2/////////////////////////
  but2State = digitalRead(but2);

  if (but2State == HIGH) {

 }
}

void toggleswitch1(){        ////////////////////////SWITCH 1/////////////////////////
  switch1state = digitalRead(switch1);
  //Serial.println(switch1state);

  if (switch1state == HIGH) {
      timesFactor = 1;
      units = 'A';
  }
  else{
      timesFactor = 240;
      units = 'W';
  }

}void postWriteEEPROMmove(){ 
  addr = addr + 1;
  if (addr == EEPROM.length()) {
    addr = 0;
  }
}
