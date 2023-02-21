/* -FREE RTOS with Tasks
 * -SPIFFS for saving settiings
 * -Single IR control so far
 * -433 Radio receive
 */

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

//#include <LITTLEFS.h> 
//#include <EEPROM.h>
#include "SPIFFS.h" 
#include "FS.h"
//#include <IRremote.h>
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

//  10554529   UP
//  10554530  STOP
//  10554532 DWN



#define FORMAT_SPIFFS_IF_FAILED true

const int B_Up = 34;    // Up button
const int B_Dwn = 35;   // Dwn button
const int B_Rec_IR = 32;  //Record IR button
const int B_Set_Up_time = 33;    // Set up Time pin
const int B_Set_Dwn_time = 25;    // Set Down Time pin

const int SW_11_IR_in = 26;   //Switch for 1 button IR up/dwn control
const int SW_12_IR_in = 27;   //Switch for 2 button IR up/dwn control

const int ledPin = 4;      // the number of the LED pin

const int IR_in = 15;   //IR demoduated receive pin



// RELAYS //
const int R_Up = 14;    //Relay 1 TV UP
const int R_Dwn = 12;   //Relay 2 TV DWN
const int RLID_Up = 23;    //Relay 3 LID OPEN
const int RLID_Dwn = 22;   //Relay 4 LID CLOSE

// LIMIT SWITCHES //
const int LSW_lidOpen = 21;    //Limit switch reading lid open N/O
const int LSW_lidClose = 19;    //Limit switch reading lid open N/O
const int LSW_lifterDown = 13;    //Limit switch reading lid open N/O

const int AUX_B_Up = 17;    //External button 1 UP
const int  AUX_B_Dwn = 16;    // External Button 2 DWN
const int  radioReceive = 2;    // Radio Receiver pin

//IRrecv IrReceiver(IR_in);

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int Set_Up_Bstate;             // the current reading from the input pin
int lastSet_Up_Bstate = LOW;   // the previous reading from the input pin
int Set_Dwn_Bstate;             // the current reading from the input pin
int lastSet_Dwn_Bstate = LOW;   // the previous reading from the input pin


int timer1;


unsigned long Set_Up_lastDebounce = 0;  // the last time the output pin was toggled
unsigned long Set_Dwn_lastDebounce = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long startUPTime;
float UPinterval;
unsigned long startDWNTime;
float DWNinterval;

unsigned long runUPTime;
unsigned long runDwnTime;
unsigned long runLID_UpTime;
unsigned long runLID_DwnTime;

unsigned long B_Up_DebounceTime = 0;
int B_Up_buttonState;             
int B_Up_lastButtonState = LOW;  

unsigned long B_Dwn_DebounceTime = 0;
int B_Dwn_buttonState;             
int B_Dwn_lastButtonState = LOW; 

unsigned long LSW_lidOpen_DebounceTime = 0;
int LSW_lidOpen_buttonState;             
int LSW_lidOpen_lastButtonState = LOW;   

unsigned long LSW_lidClose_DebounceTime = 0;
int LSW_lidClose_buttonState;             
int LSW_lidClose_lastButtonState = LOW; 

unsigned long LSW_lifterDown_DebounceTime = 0;
int LSW_lifterDown_buttonState;             
int LSW_lifterDown_lastButtonState = LOW; 

unsigned long AUX_B_Up_DebounceTime = 0;
int AUX_B_Up_buttonState;             
int AUX_B_Up_lastButtonState = LOW; 

unsigned long AUX_B_Dwn_DebounceTime = 0;
int AUX_B_Dwn_buttonState;             
int AUX_B_Dwn_lastButtonState = LOW; 


unsigned long B_Rec_IR_DebounceTime = 0;
int B_Rec_IR_buttonState;             
int B_Rec_IR_lastButtonState = LOW;   
long unsigned int IR_code1;
long unsigned int IR_code2;

bool startUPTimer = false;
bool startDWNTimer = false;

bool R_Up_on = false;  // relay on indicator
bool R_Dwn_on = false;  // relay on indicator

bool R_lidOpen = false; // relay on indicator
bool R_lidClose = false; // relay on indicator
bool limit_lidOPEN = false;  // limit switch tripped indicator
bool limit_lidCLOSED = false; // limit switch tripped indicator
bool limit_TVdown = false; // limit switch tripped indicator


bool IR_record = false;

int B_Set_Up_timersetting = 36;  //UP max Travel time for lifter
int B_Set_Dwn_timersetting = 60; //DWN max Travel time for lifter

int B_Set_LIDUp_timersetting = 20; //UP max Travel time for lid
int B_Set_LIDDwn_timersetting = 20; //DWN max Travel time for lid

String RadioString = "";

String Load_Settings ="";
String Load_Up_time = "";
String Load_Dwn_time = "";

String Load_Up_code = "";
String Load_Dwn_code = "";

String Load_IR_code1 = "";
String Load_IR_code2 = "";

int lastdirection = 0;

void checklimits(){
  int readingX = digitalRead(LSW_lidOpen);
  if (readingX == LOW){
    limit_lidOPEN = false;
  }
  else{limit_lidOPEN = true;}
  Serial.println("Lid_Open status = " + String(limit_lidOPEN));

  readingX = digitalRead(LSW_lidClose);
  if (readingX == LOW){
    limit_lidCLOSED = false;
  }
  else{limit_lidCLOSED = true;}
  Serial.println("Lid_Closed status = " + String(limit_lidCLOSED));

  readingX = digitalRead(LSW_lifterDown);
  if (readingX == LOW){
    limit_TVdown = false;
  }
  else{limit_TVdown = true;}
  Serial.println("Lifter status = " + String(limit_TVdown));
}

void lidOPEN(){
    R_lidClose = false; // Ensure Both relays do not run at same time
    digitalWrite(RLID_Dwn, LOW); //de-activate relay
    Serial.println("");
    Serial.println("Lid Open Relay ON");
    digitalWrite(RLID_Up, HIGH); //activate relay
    digitalWrite(ledPin, HIGH);
    R_lidOpen = true; //lid relay bool
    
    unsigned long time_now = 0;
    time_now = millis();
    timer1 = B_Set_LIDUp_timersetting*1000;
    Serial.println("Travel timeout: ["+ String(timer1)+ "] started");
    while((millis() - time_now < timer1) && (limit_lidOPEN == false)){
         if ((millis() - LSW_lidOpen_DebounceTime) > debounceDelay) {
          int reading = digitalRead(LSW_lidOpen);
          if (reading != LSW_lidOpen_buttonState) {
            LSW_lidOpen_buttonState = reading;
            if (LSW_lidOpen_buttonState == HIGH) {
              Serial.println("LID is fully open");
              limit_lidOPEN = true; //LID is fully open
              digitalWrite(RLID_Up, LOW);
              digitalWrite(ledPin, LOW);
              R_lidOpen = false;
              Serial.println("Lid Open Relay OFF");
              //lifterUP(); // now lift TV
              
              }
            }
          } 
         delay(100);  

       }
       Serial.println("Timer completed");
       digitalWrite(RLID_Up, LOW);
       digitalWrite(ledPin, LOW);
}

void lidCLOSE(){
  if (limit_TVdown == true){
    R_lidOpen = false; // Ensure Both relays do not run at same time
    digitalWrite(RLID_Up, LOW); //de-activate relay
    Serial.println("");
    Serial.println("Lid Close Relay ON");
    digitalWrite(RLID_Dwn, HIGH); //activate relay
    digitalWrite(ledPin, HIGH);
    R_lidClose = true;
        
    unsigned long time_now = 0;
    time_now = millis();
    timer1 = B_Set_LIDDwn_timersetting*1000;
    Serial.println("Travel timeout: ["+ String(timer1)+ "] started");
    while((millis() - time_now < timer1) && (limit_lidCLOSED == false)){
           if ((millis() - LSW_lidClose_DebounceTime) > debounceDelay) {
            int reading = digitalRead(LSW_lidClose);
            if (reading != LSW_lidClose_buttonState) {
              LSW_lidClose_buttonState = reading;
              if (LSW_lidClose_buttonState == HIGH) {
                Serial.println("LID is fully closed");
                limit_lidCLOSED = true; //LID is fully closed
                digitalWrite(RLID_Dwn, LOW);
                digitalWrite(ledPin, LOW);
                R_lidClose = false;
                Serial.println("Lid Closed Relay OFF");  
              }
            }
          } 
         delay(100);  

       }
       Serial.println("Timer completed");
       digitalWrite(RLID_Dwn, LOW);
       digitalWrite(ledPin, LOW);
}

  else{
      //do not continue
      Serial.println("Cannot comply- Lifter not yet retracted");
  }
}

void lifterUP(){
  R_Dwn_on = false; // Ensure Both relays do not run at same time
  digitalWrite(R_Dwn, LOW);
  Serial.println("");
  Serial.println("Lifter Up Relay ON");
  digitalWrite(R_Up, HIGH);
  digitalWrite(ledPin, HIGH);
  R_Up_on = true;
  lastdirection = 1;  // 1 for up 0 for down, -1 for error
  
  unsigned long time_now = 0;
  time_now = millis();
  timer1 = B_Set_Up_timersetting*1000;
  Serial.println("Travel timeout: ["+ String(timer1)+ "] started");
  while((millis() - time_now < timer1)){
    //do nothing as lifter is moving
  }
  Serial.println("Lifter Up Relay OFF");
  digitalWrite(R_Up, LOW);
  digitalWrite(ledPin, LOW);
  R_Up_on = false;
  
}

void lifterDwn(){
  R_Up_on = false; // Ensure Both relays do not run at same time
  digitalWrite(R_Up, LOW);

  Serial.println("");
  Serial.println("Lifter Dwn Relay ON");
  digitalWrite(R_Dwn, HIGH);
  digitalWrite(ledPin, HIGH);
  R_Dwn_on = true;
  lastdirection = 0;  // 1 for up 0 for down, -1 for error
  
  unsigned long time_now = 0;
  time_now = millis();
  timer1 = B_Set_Dwn_timersetting*1000;
  Serial.println("Travel timeout: ["+ String(timer1)+ "] started");
  while((millis() - time_now < timer1) && (limit_TVdown == false)){
    if ((millis() - LSW_lifterDown_DebounceTime) > debounceDelay) {
      int reading = digitalRead(LSW_lifterDown);
      if (reading != LSW_lifterDown_buttonState) {
        LSW_lifterDown_buttonState = reading;
        if (LSW_lifterDown_buttonState == HIGH) {
          Serial.println("Lifter is fully down- limit switch trigger");
          limit_TVdown = true;
          }
        }
      } 
     delay(100);  
  }
  Serial.println("Lifter Down");
  Serial.println("Lifter Dwn Relay OFF");
  digitalWrite(R_Dwn, LOW);
  digitalWrite(ledPin, LOW);
  R_Dwn_on = false;

}

void fullyopen(){
  Serial.println("Begin open sequence");
  if (limit_lidOPEN == false){ //if the lid is not yet fully open then run that first
    lidOPEN();
  }
  else{
    Serial.println("Lid open Limit = "+ String(limit_lidOPEN));
  }
 
 lifterUP(); // now that the lid is open- raise the lifter
}


void fullyclose(){
  Serial.println("Begin close sequence");
  lifterDwn();
  if (limit_TVdown == true){ // only if the lifter is fully down do we close the lid
    lidCLOSE();
  }
  else{
    Serial.println("Lifter Limit = "+ String(limit_TVdown));
    Serial.println("Lifter not down- Cannot close lid");
  }
}

void stopUnit(){
  Serial.println("STOP ALL");
  digitalWrite(R_Dwn, LOW);
  digitalWrite(R_Up, LOW);
  digitalWrite(RLID_Up, LOW);
  digitalWrite(RLID_Dwn, LOW);
  digitalWrite(ledPin, LOW);

  R_Up_on = false;  // relay on indicator
  R_Dwn_on = false;  // relay on indicator
  R_lidOpen = false; // relay on indicator
  R_lidClose = false; // relay on indicator
}

// define tasks 
void Task_readRadio( void *pvParameters );
void Task_TvliftUp( void *pvParameters );
void Task_TvliftDwn( void *pvParameters );
//void Task_Tset_up( void *pvParameters );
//void Task_Tset_dwn( void *pvParameters );
//void Task_RecIRCode( void *pvParameters );

void Task_readExtUp_but( void *pvParameters );
void Task_readExtDwn_but( void *pvParameters );

void setup() {

  Serial.begin(115200);
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
        Serial.println("SPIFFS Mount Failed");
        return;
    }
  mySwitch.enableReceive(radioReceive);  
  Serial.println("");  
  Serial.println("<INIT>");

  
  Serial.print("Loaded: [");
  Serial.print(Load_Settings);
  Serial.println("]");



  pinMode(B_Up, INPUT);
  pinMode(B_Dwn, INPUT);
//  pinMode(B_Rec_IR, INPUT);
  pinMode(B_Set_Up_time, INPUT);
  pinMode(B_Set_Dwn_time, INPUT);
//  pinMode(SW_11_IR_in, INPUT);
//  pinMode(SW_12_IR_in, INPUT);
  //pinMode(IR_in, INPUT);
  pinMode(AUX_B_Up, INPUT); // AUX switch
  pinMode(AUX_B_Dwn, INPUT); // AUX switch

  pinMode(ledPin, OUTPUT);  //led
  pinMode(R_Up, OUTPUT);  // Relay
  pinMode(R_Dwn, OUTPUT);  // Relay
  pinMode(RLID_Up, OUTPUT);  // Relay
  pinMode(RLID_Dwn, OUTPUT); // Relay

  pinMode(LSW_lidOpen, INPUT);  // Limit switch
  pinMode(LSW_lidClose, INPUT);  // Limit switch
  pinMode(LSW_lifterDown, INPUT);  // Limit switch


  checklimits(); // check limtis

  // set initial LED state
  digitalWrite(ledPin, LOW);


//  IrReceiver.enableIRIn();  // Start the receiver
//  IrReceiver.blink13(false); // Enable feedback LED
  Serial.println("\n"); 

  
  // Set up tasks to run independently.
  xTaskCreatePinnedToCore(
    Task_readRadio
    ,  "Task_readExtUp_but"
    ,  1024  // Stack size
    ,  NULL
    ,  3  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
  
  xTaskCreatePinnedToCore(
    Task_TvliftUp
    ,  "Task_TvliftUp"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    Task_TvliftDwn
    ,  "Task_TvliftDwn"
    ,  1024  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    Task_readExtUp_but
    ,  "Task_readExtUp_but"
    ,  1024  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    Task_readExtDwn_but
    ,  "Task_readExtDwn_but"
    ,  1024  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

//  xTaskCreatePinnedToCore(
//    Task_Tset_up
//    ,  "Task_Tset_up"
//    ,  1024  // Stack size
//    ,  NULL
//    ,  2  // Priority
//    ,  NULL 
//    ,  ARDUINO_RUNNING_CORE);
//
//  xTaskCreatePinnedToCore(
//    Task_Tset_dwn
//    ,  "Task_Tset_dwn"
//    ,  1024  // Stack size
//    ,  NULL
//    ,  2  // Priority
//    ,  NULL 
//    ,  ARDUINO_RUNNING_CORE);
//
//  xTaskCreatePinnedToCore(
//    Task_RecIRCode
//    ,  "Task_RecIRCode"
//    ,  1024  // Stack size
//    ,  NULL
//    ,  3  // Priority
//    ,  NULL 
//    ,  ARDUINO_RUNNING_CORE);




  // Now the task scheduler is automatically started.
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

// RADIO SIGNAL DETECTION
void Task_readRadio(void *pvParameters){  // Task0
  (void) pvParameters;
  for (;;){
    if (mySwitch.available()) {
        RadioString = mySwitch.getReceivedValue();
        
        Serial.print("Received ");
        Serial.print( RadioString );
        Serial.print(" / ");
        Serial.print( mySwitch.getReceivedBitlength() );
        Serial.print("bit ");
        Serial.print("Protocol: ");
        Serial.println( mySwitch.getReceivedProtocol() );
    
        if(RadioString.length()>0){
          if(RadioString == "10554529"){
            Serial.println("");
            Serial.println("*REC UP CMD*");
            delay(50);
            checklimits();
            fullyopen();
            Serial.println("REC UP COMPLETE");
          }
          else if(RadioString == "10554530"){
            Serial.println("");
            Serial.println("*REC STOP CMD*");
            delay(50);
            stopUnit();
            checklimits();
          }
          else if(RadioString == "10554532"){
            Serial.println("");
            Serial.println("*REC DWN CMD*");
            delay(50);
            checklimits();
            fullyclose();
            Serial.println("REC DWN COMPLETE");
          }
          else{
            } 
        }
        mySwitch.resetAvailable();
      }
  }
}


// ONBOARD UP BUTTON PRESS
void Task_TvliftUp(void *pvParameters){  // Task1
  (void) pvParameters;
  for (;;){ // A Task shall never return or exit.
    int reading = digitalRead(B_Up);
    if (reading != B_Up_lastButtonState) {
      B_Up_DebounceTime = millis();
    }
  
    if ((millis() - B_Up_DebounceTime) > debounceDelay) {
      if (reading != B_Up_buttonState) {
        B_Up_buttonState = reading;
        if (B_Up_buttonState == HIGH) {
          if(R_Up_on == false){
            Serial.println("UP");
//            runUPTime = millis ();
            R_Up_on = true;
          }
   
        }
      }
    }
    if(R_Up_on == true){
        lifterUP();
      }
    B_Up_lastButtonState = reading;
  }
}

// ONBOARD DWN BUTTON PRESS
void Task_TvliftDwn(void *pvParameters){  // Task2
  (void) pvParameters;
  for (;;){
    int reading = digitalRead(B_Dwn);
    if (reading != B_Dwn_lastButtonState) {
      B_Dwn_DebounceTime = millis();
    }
    if ((millis() - B_Dwn_DebounceTime) > debounceDelay) {
      if (reading != B_Dwn_buttonState) {
        B_Dwn_buttonState = reading;
        if (B_Dwn_buttonState == HIGH) {
          if(R_Dwn_on == false){
            Serial.println("DWN");
//            runDwnTime = millis ();
            R_Dwn_on = true;
          }
        }
      }
    }
    if(R_Dwn_on == true){
      lifterDwn();
      }
    B_Dwn_lastButtonState = reading;
  }
}

// AUX UP BUTTON PRESS
void Task_readExtUp_but(void *pvParameters){  // Task
  (void) pvParameters;
  for (;;){

    int reading = digitalRead(AUX_B_Up);
    if (reading != AUX_B_Up_lastButtonState) {
      AUX_B_Up_DebounceTime = millis();
    }
    if ((millis() - AUX_B_Up_DebounceTime) > debounceDelay) {
      if (reading != AUX_B_Up_buttonState) {
        AUX_B_Up_buttonState = reading;
        if (AUX_B_Up_buttonState == HIGH) {
          Serial.println("AUX UP CMD");
        }
    
//    Serial.println(digitalRead(AUX_B_Up));
//    if(digitalRead(AUX_B_Up)== LOW){
//      //lastdirection = 1;  // 1 for up 0 for down, -1 for nil
//      Serial.println("Ext Up press");
      }
    }
  }
}

// AUX DWN BUTTON PRESS
void Task_readExtDwn_but(void *pvParameters){  // Task
  (void) pvParameters;
  for (;;){

    int reading = digitalRead(AUX_B_Dwn);
    if (reading != AUX_B_Dwn_lastButtonState) {
      AUX_B_Dwn_DebounceTime = millis();
    }
    if ((millis() - AUX_B_Dwn_DebounceTime) > debounceDelay) {
      if (reading != AUX_B_Dwn_buttonState) {
        AUX_B_Dwn_buttonState = reading;
        if (AUX_B_Dwn_buttonState == HIGH) {
          Serial.println("AUX DWN CMD");
        }
   
      }
    }
  }
}


//void Task_Tset_up(void *pvParameters){  // Task3
//  (void) pvParameters;
//  for (;;){
//    int reading = digitalRead(B_Set_Up_time);
//    if (reading != lastSet_Up_Bstate) {
//      // reset the debouncing timer
//      Set_Up_lastDebounce = millis();
//    }
//  
//    if ((millis() - Set_Up_lastDebounce) > debounceDelay) {
//      if (reading != Set_Up_Bstate) {
//        Set_Up_Bstate = reading;
//        if (Set_Up_Bstate == HIGH) {
//  
//          if(startUPTimer == false){
//            Serial.println("UP Timer Started");
//            startUPTimer = true;
//            digitalWrite(ledPin, HIGH);
//            startUPTime = millis ();
//          }
//          
//          else{
//            Serial.println("UP Timer Stopped");
//            startUPTimer = false;
//  
//            digitalWrite(ledPin, LOW);
//            UPinterval = millis () - startUPTime;
//            B_Set_Up_timersetting = float(UPinterval)/1000;
//            Serial.print("New Up setting:");
//            Serial.print(B_Set_Up_timersetting);
//            Serial.println(" seconds");
//            
//            File UP_file = SPIFFS.open("/Uptime.txt", FILE_WRITE);
//            UP_file.print((B_Set_Up_timersetting*1000));
//            UP_file.close();
//            
//          }
//          
//        }
//      }
//    }
//  
//    lastSet_Up_Bstate = reading;
//  }
//}
//
//void Task_Tset_dwn(void *pvParameters){  // Task4
//  (void) pvParameters;
//  for (;;){
//    int Dwnreading = digitalRead(B_Set_Dwn_time);
//    if (Dwnreading != lastSet_Dwn_Bstate) {
//      // reset the debouncing timer
//      Set_Dwn_lastDebounce = millis();
//    }
//  
//    if ((millis() - Set_Dwn_lastDebounce) > debounceDelay) {
//      if (Dwnreading != Set_Dwn_Bstate) {
//        Set_Dwn_Bstate = Dwnreading;
//        if (Set_Dwn_Bstate == HIGH) {
//  
//          if(startDWNTimer == false){
//            Serial.println("Dwn Timer Started");
//            startDWNTimer = true;
//            digitalWrite(ledPin, HIGH);
//            startDWNTime = millis ();
//          }
//          
//          else{
//            Serial.println("DWN Timer Stopped");
//            startDWNTimer = false;
//  
//            digitalWrite(ledPin, LOW);
//            DWNinterval = millis () - startDWNTime;
//            B_Set_Dwn_timersetting = float(DWNinterval)/1000;
//            Serial.print("New Dwn setting:");
//            Serial.print(B_Set_Dwn_timersetting);
//            Serial.println(" seconds");
//  
//            File DWN_file = SPIFFS.open("/Dwntime.txt", FILE_WRITE);
//            DWN_file.print((B_Set_Dwn_timersetting*1000));
//            DWN_file.close();
//          }
//        }
//      }
//    }
//    lastSet_Dwn_Bstate = Dwnreading;
//
//  }
//}
//
//void Task_RecIRCode(void *pvParameters){  // Task5
//  (void) pvParameters;
//  for (;;){
//    int reading = digitalRead(B_Rec_IR); //B_Rec_IR_
//    if (reading != B_Rec_IR_lastButtonState) {
//      B_Rec_IR_DebounceTime = millis();
//    }
//  
//    if ((millis() - B_Rec_IR_DebounceTime) > debounceDelay) {
//      if (reading != B_Rec_IR_buttonState) {
//        B_Rec_IR_buttonState = reading;
//        if (B_Rec_IR_buttonState == HIGH) {
//          Serial.println("Recording IR Code");
//          IR_record = true;
//          Serial.println("Enabling IRin");
//  //        IrReceiver.enableIRIn();  // Start the receiver
//  //        IrReceiver.blink13(false); // Enable feedback LED
//        }
//      }
//    }
//    
//    if(IrReceiver.decode()){
//          //IrReceiver.printResultShort(&Serial);
//          long unsigned int datain = IrReceiver.results.value;
//          //datain = datain;
//          //Serial.println();
//          Serial.print("<");
//          //Serial.print(datain, HEX);
//          Serial.print(datain);
//          Serial.println(">");
//  
//          
//          if (IR_record == true) {
//            //IR_code1 = datain, HEX;
//            IR_code1 = datain;
//            IR_record = false;
//    
//            File IR_code1_file = SPIFFS.open("/IR_code1.txt", FILE_WRITE);
//            Serial.print("Save String: ");
//            //Serial.println(datain, HEX);
//            Serial.println(datain);
//            //IR_code1_file.print("<");
//            //IR_code1_file.print(datain, HEX);
//            IR_code1_file.print(datain);
//            //IR_code1_file.print(">");
//            IR_code1_file.close();
//          
//          }
//          else{
//            Serial.println(IR_code1);
//            Serial.println(datain);
//            if(datain == IR_code1){
//              Serial.println("<Received Correct IR>");
//  
//              if (lastdirection == 0){  // 1 for up 0 for down, -1 for nil
//                Serial.println("<R_Up ON>");
//                R_Up_on = true;
//                runUPTime = millis ();
//              }
//              else if (lastdirection == 1){
//                Serial.println("<R_Dwn ON>");
//                R_Dwn_on = true;
//                runDwnTime = millis ();
//              }
//              else{
//                Serial.println("<ERROR in direction>");
//                }
//            }
//          }
//    IrReceiver.resume(); // Receive the next value
//    
//    }
//    B_Rec_IR_lastButtonState = reading;
//  }
//}







//void Task_XXX(void *pvParameters){  // Task8
//  (void) pvParameters;
//  for (;;){
//
//  }
//}
