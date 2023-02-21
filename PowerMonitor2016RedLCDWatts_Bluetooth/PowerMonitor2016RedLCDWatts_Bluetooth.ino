#define cs   53
#define dc   47  //RS Pin TFT
#define rst  48  // you can also connect this to the Arduino reset
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>
#include "EmonLib.h"                   // Include Emon Library
#include "Wire.h"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);
const int lcd2 = 49;    //LCD Backlight Pin for TFT
const int chipSelect = 46;   //SD CARD SELECT      /////////PIN 4 for ETHERCARD
const int but1= 5;
const int but2 = 6;
int but1var = 1;
int ledState = HIGH;        
int but1State = 0;  
int mainDelay = 10000;   ///////////////MAIN DELAY TIME SETTING
int counterX = 0; ////// Counter for writing files after 1 hour


// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers



#define   FILL1 tft.fillRect(1, 1, 124, 7, ST7735_BLACK)  //TIME BLANK
#define   FILL2 tft.fillRect(1, 13, 124, 143, ST7735_BLACK)  //BODY BLANK
#define   SQUARE0 tft.drawRect(0, 20, 126, 45, ST7735_WHITE) //INITIAL WHITE SQUARE
#define   SQUARE1 tft.drawRect(0, 10, 126, 148, ST7735_WHITE) //BODY WHITE SQUARE
#define   SQUARE2 tft.drawRect(0, 0, 126, 11, ST7735_WHITE) //TIME WHITE SQUARE


int LcdReset = 34;
File dataFile;
EnergyMonitor emon1, emon2, emon3, emon4, emon5, emon6, emon7, emon8, emon9, emon10, emon11, emon12, emon13, emon14;   
// Create 14 instances

#define DS1307_ADDRESS 0x68// RTC I2C Address

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
 
 float Sval1x;
 float Sval2x;
 float Sval3x;
 float Sval4x;
 float Sval5x;
 float Sval6x;
 float Sval7x;
 float Sval8x;
 float Sval9x;
 float Sval10x;
 float Sval11x;
 float Sval12x;
 float Sval13x;
 float Sval14x;


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

 void setup() {//////////////////////////////////////////////SETUP//////////////////////////////////
  // pinMode(Power, OUTPUT);
  pinMode(LcdReset, OUTPUT);
  pinMode(lcd2, OUTPUT);    //RS pin For TFT
  pinMode(but1, INPUT);
  pinMode(but2, INPUT);

    Serial.begin(9600);
    Sensor();   /////////// Inital Sensor run to clear invalid sensr values '45'
    


  tft.initR(INITR_REDTAB);   // initialize a ST7735R chip, red tab
  tft.setRotation(2);        //TFT Screen Orientation
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.invertDisplay(false);
  delay(30);
 // lcd2, LOW;  //Back Light PIN ON TFT                           ///////change this code- it is worng
  digitalWrite(lcd2, HIGH);                       ///////USE THIS ONE FOR SMALL RED SCREENS
  /*
  tft.setCursor(0, 30);
   tft.setTextColor(ST7735_WHITE);
   tft.setTextSize(1);
   tft.println("    Power Monitor");
   tft.setCursor(0, 45);
   tft.println("      Initlized");
   delay(1000);
   */

  lcd.begin(16,2); 
  delay(10);
  lcd.backlight();

  lcd.clear();
  ////////////////////////////////////LCD1 message///////////////////////////////
  
  lcd.setCursor(0,0);
  SQUARE0;
  lcd.print("Initializing SD"); 
  Serial.print("Initializing SD card...");


  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
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
    while (1) ;
  }
  Serial.println("card initialized.");
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
  dataFile = SD.open("datalog.csv", FILE_WRITE);
  if (! dataFile) {
    Serial.println("error opening datalog.csv");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Failed to open");

    tft.setCursor(0, 30);
    tft.fillScreen(ST7735_BLACK);
    SQUARE0;
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("      Failed to ");
    tft.setCursor(0, 45);
    tft.println("       open file");


    // Wait forever since we cant write data



    while (1) ;
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
  
 


/*
Sval1 = 0;
Sval2 = 0;
Sval3 = 0;
Sval4 = 0;
Sval5 = 0;
Sval6 = 0;
Sval7 = 0;
Sval8 = 0;
Sval9 = 0;
Sval10 = 0;
Sval11 = 0;
Sval12 = 0;
Sval13 = 0;
Sval14 = 0;
*/

SQUARE1;
SQUARE2;
FILL2;

 }




 void loop() {      /////////////////////////////////////LOOP////////////////////////////////////
 
 button1();

//  dataFile = SD.open("datalog.csv", FILE_WRITE);
//  if (dataFile != SD.open("datalog.csv", FILE_WRITE))
    dataFile = SD.open("datalog.csv", FILE_WRITE);
  if (! dataFile) 
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.println("Out Of Sync"); 
    tft.setCursor(0, 30);
    tft.fillScreen(ST7735_BLACK);
    FILL2;
    SQUARE0;
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("     Out of SYNC ");
    tft.println("   Re-insert card");
    // don't do anything more:
    //while (1) ;
  delay(10000);  // Allow time for card to be inserted
}

printDate();

  
  if (counterX >= 360){
     dataFile = SD.open("datalog.csv", FILE_WRITE);
     printDate2();
      
    dataFile.print(Sval1x / counterX);
    dataFile.print(", ");
    dataFile.print(Sval2x / counterX);
    dataFile.print(", ");
    dataFile.print(Sval3x / counterX);
    dataFile.print(", ");
    dataFile.print(Sval4x / counterX);
    dataFile.print(", ");
    dataFile.print(Sval5x / counterX);
    dataFile.print(", ");
    dataFile.print(Sval6x / counterX);
    dataFile.print(", ");
    dataFile.print(Sval7x / counterX);
    dataFile.print(", ");
    dataFile.print(Sval8x / counterX);
    dataFile.print(", ");
    dataFile.print(Sval9x / counterX);
    dataFile.print(", ");
    dataFile.print(Sval10x / counterX);
    dataFile.print(", ");
    dataFile.print(Sval11x / counterX);
    dataFile.print(", ");
    dataFile.print(Sval12x / counterX);
    dataFile.print(", ");
    dataFile.print(Sval13x / counterX);
    dataFile.print(", ");
    dataFile.println(Sval14x / counterX);
    
    dataFile.flush();
    dataFile.close();
    
    Sval1x = 0;
    Sval2x = 0;
    Sval3x = 0;
    Sval4x = 0;
    Sval5x = 0;
    Sval6x = 0;
    Sval7x = 0;
    Sval8x = 0;
    Sval9x = 0;
    Sval10x = 0;
    Sval11x = 0;
    Sval12x = 0;
    Sval13x = 0;
    Sval14x = 0;
    
    counterX =0;

    
   
     
  } 
  
   
  
  Sensor();
  
    Sval1 = Sval1 * 230;
    Sval2 = Sval2 * 230;
    Sval3 = Sval3 * 230;
    Sval4 = Sval4 * 230;
    Sval5 = Sval5 * 230;
    Sval6 = Sval6 * 230;
    Sval7 = Sval7 * 230;
    Sval8 = Sval8 * 230;
    Sval9 = Sval9 * 230;
    Sval10 = Sval10 * 230;
    Sval11 = Sval11 * 230;
    Sval12 = Sval12 * 230;
    Sval13 = Sval13 * 230;
    Sval14 = Sval14 * 230;


  lcd.clear();
  lcd.setCursor(0,0);
  Serial.print ("<");
  Serial.print (Sval1);
  Serial.print (", ");
  lcd.print(Sval1,0);
  lcd.print(",");
  Sval1x = Sval1x + Sval1;
  
  Serial.print (Sval2);
  Serial.print (", ");
  lcd.print(Sval2,0);
  lcd.print(",");
  Sval2x = Sval2x + Sval2;
  
  Serial.print (Sval3);
  Serial.print (", ");
  lcd.print(Sval3,0);
  lcd.print(",");
  Sval3x = Sval3x + Sval3;
  
  Serial.print (Sval4);
  Serial.print (", ");
  lcd.print(Sval4,0);
  lcd.print(",");
  Sval4x = Sval4x + Sval4;
  
  Serial.print (Sval5);
  Serial.print (", ");
  lcd.print(Sval5,0);
  //  lcd.print(",");
  Sval5x = Sval5x + Sval5;
  
  Serial.print (Sval6);
  Serial.print (", ");
  lcd.setCursor(0,1);
  lcd.print(Sval6,0);
  lcd.print(",");
  Sval6x = Sval6x + Sval6;
  
  Serial.print (Sval7);
  Serial.print (", ");
  lcd.print(Sval7,0);
  lcd.print(","); 
  Sval7x = Sval7x + Sval7;
  
  Serial.print (Sval8);
  Serial.print (", ");
  lcd.print(Sval8,0);
  lcd.print(",");
  Sval8x = Sval8x + Sval8;
  
  Serial.print (Sval9);
  Serial.print (", ");
  lcd.print(Sval9,0);
  lcd.print(",");
  Sval9x = Sval9x + Sval9;
  
  Serial.print (Sval10);
  Serial.print (", ");
  lcd.print(Sval10,0);
  lcd.print(",");
  Sval10x = Sval10x + Sval10;
  
  Serial.print (Sval11);
  Serial.print (", ");
  lcd.print(Sval11,0);
  lcd.print(",");
  Sval11x = Sval11x + Sval11;
  
  Serial.print (Sval12);
  Serial.print (", ");
  lcd.print(Sval12,0);
  lcd.print(",");
  Sval12x = Sval12x + Sval12;
  
  Serial.print (Sval13);
  Serial.print (", ");
  lcd.print(Sval13,0);
  lcd.print(",");
  Sval13x = Sval13x + Sval13;
  
  Serial.print (Sval14);
  Serial.print ("> ");
  lcd.println(Sval14,0);
  Sval14x = Sval14x + Sval14;




  

  // tft.setCursor(0,0);
  //tft.fillScreen(ST7735_BLACK);
  FILL2;
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  //  tft.println();

  /*
   tft.setCursor(0,0);
   tft.fillScreen(ST7735_BLACK);
   tft.setTextColor(ST7735_WHITE);
   tft.setTextSize(1);
   tft.print(monthDay);
   tft.print("/");
   tft.print(month);
   tft.print(" ");
   tft.print(hour);
   tft.print(":");
   tft.print(minute);
   */

  tft.setCursor(2,14);
  tft.print("Sensor 1:  ");
  tft.println(Sval1);

  tft.setCursor(2,24);
  tft.print("Sensor 2:  ");
  tft.println(Sval2);

  tft.setCursor(2,34);
  tft.print("Sensor 3:  ");
  tft.println(Sval3);

  tft.setCursor(2,44);
  tft.print("Sensor 4:  ");
  tft.println(Sval4);

  tft.setCursor(2,54);
  tft.print("Sensor 5:  ");
  tft.println(Sval5);

  tft.setCursor(2,64);
  tft.print("Sensor 6:  ");
  tft.println(Sval6);

  tft.setCursor(2,74);
  tft.print("Sensor 7:  ");
  tft.println(Sval7);

  tft.setCursor(2,84);
  tft.print("Sensor 8:  ");
  tft.println(Sval8);

  tft.setCursor(2,94);
  tft.print("Sensor 9:  ");
  tft.println(Sval9);

  tft.setCursor(2,104);
  tft.print("Sensor 10: ");
  tft.println(Sval10);

  tft.setCursor(2,114);
  tft.print("Sensor 11: ");
  tft.println(Sval11);

  tft.setCursor(2,124);
  tft.print("Sensor 12: ");
  tft.println(Sval12);  

  tft.setCursor(2,134);
  tft.print("Sensor 13: ");
  tft.println(Sval13);
  
  tft.setCursor(2,144);
  tft.print("Sensor 14: ");
  tft.println(Sval14);

  counterX ++;
  
  Serial.println (counterX);
  
  delay(mainDelay);      /////////////////////Main Delay///////////////////////


}

byte bcdToDec(byte val)  {
  // Convert binary coded decimal to normal decimal numbers
  return ( (val/16*10) + (val%16) );
}





void printDate(){   /////////////////////RTC TIMER////////////////////////

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


  lcd.clear();
  lcd.setCursor(0,0);

  Serial.print ("[");
  Serial.print (monthDay);
  Serial.print ("/");
  lcd.print(monthDay);
  lcd.print("/");

  Serial.print (month);
  Serial.print (" ");
  lcd.print(month);
  lcd.print(" ");

  Serial.print (hour);
  Serial.print (":");
  lcd.print(hour);
  lcd.print(":");

  Serial.print (minute);
  //Serial.print (", ");
  Serial.print ("] ");
  lcd.print(minute);
  lcd.print(", ");


  FILL1;
  tft.setCursor(0,2);
  //tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.print("      ");
  tft.print(monthDay);
  tft.print("/");
  tft.print(month);
  tft.print(" ");
  tft.print(hour);
  tft.print(":");
  tft.print(minute);



 }



/////////////////////// DATE 2 For writing////////////////////
void printDate2(){   /////////////////////RTC TIMER////////////////////////

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


  lcd.clear();
  lcd.setCursor(0,0);

  Serial.print (monthDay);
  Serial.print ("/");
  lcd.print(monthDay);
  lcd.print("/");
  dataFile.print(monthDay);
  dataFile.print("/");  
  Serial.print (month);
  Serial.print (" ");
  lcd.print(month);
  lcd.print(" ");
  dataFile.print(month);
  dataFile.print(" ");
  Serial.print (hour);
  Serial.print (":");
  lcd.print(hour);
  lcd.print(":");
  dataFile.print(hour);
  dataFile.print(":"); 
  Serial.print (minute);
  Serial.print (", ");
  lcd.print(minute);
  lcd.print(", ");
  dataFile.print(minute);
  dataFile.print(", ");

  FILL1;
  tft.setCursor(0,2);
  //tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.print("      ");
  tft.print(monthDay);
  tft.print("/");
  tft.print(month);
  tft.print(" ");
  tft.print(hour);
  tft.print(":");
  tft.print(minute);



 }




 void Sensor(){   ////////////////////////SENSOR/////////////////////////
 
  // Channel 1:
  double Irms = emon1.calcIrms(1480);  // Calculate Irms only
  // Serial.print(Irms5*230.0);        // Apparent power
  // Serial.print ("Sensor1: ");
  // Serial.print(Irms1/1.8);          // Irms
  Sval1= Irms/1.8;
if(Sval1 <= .3){
Sval1 = 0;} 


  // Channel 2
  double Irms2 = emon2.calcIrms(1480);  // Calculate Irms only
  Sval2= Irms2/1.8;
  if(Sval2 <= .3){
Sval2 = 0;} 


  // Channel 3
  double Irms3 = emon3.calcIrms(1480);  // Calculate Irms only
  Sval3= Irms3/1.8;
  if(Sval3 <= .3){
Sval3 = 0;} 


  // Channel 4
  double Irms4 = emon4.calcIrms(1480);  // Calculate Irms only
  Sval4= Irms4/1.8;
  if(Sval4 <= .3){
Sval4 = 0;} 

  // Channel 5   
  double Irms5 = emon5.calcIrms(1480);  // Calculate Irms only
  Sval5= Irms5/1.8;
  if(Sval5 <= .3){
Sval5 = 0;} 

  // Channel 6   
  double Irms6 = emon6.calcIrms(1480);  // Calculate Irms only
  Sval6= Irms6/1.8;
  if(Sval6 <= .3){
Sval6 = 0;} 

  // Channel 7   
  double Irms7 = emon7.calcIrms(1480);  // Calculate Irms only
  Sval7= Irms7/1.8;
  if(Sval7 <= .3){
Sval7 = 0;} 

  // Channel 8   
  double Irms8 = emon8.calcIrms(1480);  // Calculate Irms only
  Sval8= Irms8/1.8;
  if(Sval8 <= .3){
Sval8 = 0;} 

  // Channel 9   
  double Irms9 = emon9.calcIrms(1480);  // Calculate Irms only
  Sval9= Irms9/1.8;
  if(Sval9 <= .3){
Sval9 = 0;} 

  // Channel 10   
  double Irms10 = emon10.calcIrms(1480);  // Calculate Irms only
  Sval10= Irms10/1.8; 
  if(Sval10 <= .3){
Sval10 = 0;} 

  // Channel 11   
  double Irms11 = emon11.calcIrms(1480);  // Calculate Irms only
  Sval11= Irms11/1.8;
 if(Sval11 <= .3){
Sval11 = 0;}  

  // Channel 12   
  double Irms12 = emon12.calcIrms(1480);  // Calculate Irms only
  Sval12= Irms12/1.8;  
  if(Sval12 <= .3){
Sval12 = 0;} 

  // Channel 13   
  double Irms13 = emon13.calcIrms(1480);  // Calculate Irms only
  Sval13= Irms13/1.8;
if(Sval13 <= .3){
Sval13 = 0;}   
  
    // Channel 14   
  double Irms14 = emon14.calcIrms(1480);  // Calculate Irms only
  Sval14= Irms14/1.8;
if(Sval14 <= .3){
Sval14 = 0;}  


 }


void button1(){        ////////////////////////BUTTON 1/////////////////////////
  but1State = digitalRead(but1);

  if (but1State == HIGH) {
   // if (lcd2 == HIGH){
      switch (but1var){
      case 1:
      digitalWrite(lcd2, LOW);
      but1var = 2;
      break;
      
      case 2:
      digitalWrite(lcd2, HIGH);
      but1var = 1;
      break;    
      }  
 }
}

