
#define R1 2   
#define R2 3
#define R3 4
#define R4 5
  
String  MSG = "";
String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete

 void setup(){
   Serial.begin(115200);
   Serial.println("<Unit Init>");

  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  
   //Setup all the Arduino Pins
   pinMode(R1, OUTPUT);
   pinMode(R2, OUTPUT);
   pinMode(R3, OUTPUT);
   pinMode(R4, OUTPUT);
   
   //Turn OFF any power to the Relay channels
   digitalWrite(R1,HIGH);
   digitalWrite(R2,HIGH);
   digitalWrite(R3,HIGH);
   digitalWrite(R4,HIGH);
 }
 
 void loop(){
  if (stringComplete) {
    //Serial.println(inputString);
    // clear the string:
    ParseString();
    inputString = "";
    stringComplete = false;
  }


 }

 void ParseString(){
      //Serial.print(MSG);
    if (MSG.indexOf("R1ON") >=0){
      digitalWrite(R1, LOW);
      Serial.println("<R1 ON>");
      }
    if (MSG.indexOf("R1OFF") >=0){
      digitalWrite(R1, HIGH);
      Serial.println("<R1 OFF>");
      }

    if (MSG.indexOf("R2ON") >=0){
      digitalWrite(R2, LOW);
      Serial.println("<R2 ON>");
      }
    if (MSG.indexOf("R2OFF") >=0){
      digitalWrite(R2, HIGH);
      Serial.println("<R2 OFF>");
      }

     if (MSG.indexOf("R3ON") >=0){
      digitalWrite(R3, LOW);
      Serial.println("<R3 ON>");
      }
    if (MSG.indexOf("R3OFF") >=0){
      digitalWrite(R3, HIGH);
      Serial.println("<R3 OFF>");
      }

     if (MSG.indexOf("R4ON") >=0){
      digitalWrite(R4, LOW);
      Serial.println("<R4 ON>");
      }
    if (MSG.indexOf("R4OFF") >=0){
      digitalWrite(R4, HIGH);
      Serial.println("<R4 OFF>");
      }

    MSG = "";

    }


 void serialEvent() {
    while (Serial.available()) {
      // get the new byte:
      char inChar = (char)Serial.read();
      // add it to the inputString:
      inputString += inChar;
      // if the incoming character is a newline, set a flag so the main loop can
      // do something about it:
      if (inChar == '\n') {
        stringComplete = true;
        MSG = inputString;
      }
    }
}

