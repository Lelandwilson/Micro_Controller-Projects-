#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/////////////////-\/ACCEL\/-//////////////////
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
/////////////////--//////////////////
const char* ssid="SSID HERE";
const char* Wifi_Password="WIFIPASSWORD HERE";

const char* www_username = "admin";
const char* www_password = "esp8266";

String webSite,javaScript,XML;
String UnitIP = "";
int Web_Port = 8088;
String Wifi_Strength = "";

int Hour_printcycle = 0;
int Hour_printlimit = 5000000;

ESP8266WebServer server(Web_Port);
/////////////////-\/VPIRs\/-//////////////////
int led_ACC = D8;                // ACC LED
int led_PIR = D7;               // PIR LED
int Sens1 = D1;               // choose the input pin (for PIR sensor1)
int Sens2 = D2;               // choose the input pin (for PIR sensor2)
int pirState1 = LOW;             // we start, assuming no motion detected
int pirState2 = LOW;             // we start, assuming no motion detected
int val1 = 0;                    // variable for reading the pin status
int val2 = 0;
int printcycle = 0;
int printlimit = 500000;
String Zone1 = "Clear";
String Zone2 = "Clear";

String EmailNotify = "Email notifications= OFF";
String AppNotify = "App notifications= OFF";
/////////////////-\/Vsens\/-//////////////////
// number of analog samples to take per reading
#define NUM_SAMPLES 100
int sum = 0;                    // sum of samples taken
unsigned char sample_count = 0; // current sample number
float voltage = 0.0;            // calculated voltage
float resistor1 = 1000000;
float resistor2 = 100000;
float denominator = (float)resistor2 / (resistor1 + resistor2);
float calibrateFactor = 1.15;
String voltageString = "Measuring";
int VoltS_printcycle = 0;
int VoltS_printlimit = 10000;

/////////////////-\/ACCEL\/-//////////////////
/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
int printcycle_ACC = 0;
int printlimit_ACC = 1000; //100 = Max sensitivity movement. 2000 = Min Sensativity 

float Tx = 0;
float Ty = 0;
float Tz = 0;

float TxPast;
float TyPast;
float TzPast;

float ps_Dif_Factor = .5; //Low = 1.5, Medium = 1, High .5
float ng_Dif_Factor = -.5; //Low = 1.5, Medium = 1, High .5
String AccelString = "Device is Stationary";

void buildWebsite(){
  buildJavascript();
  webSite="<!DOCTYPE HTML>\n";
  webSite+="<meta charset=\"UTF-8\">";
  webSite+="<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  webSite+="<link rel=\"stylesheet\" href=\"https://www.w3schools.com/w3css/4/w3.css\">";
  webSite+="<link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/css?family=Raleway\">";
  
  webSite+="<style>";
    webSite+="body,h1 {font-family: \"Raleway\", sans-serif}";
    webSite+="body, html {height: 100%}";
    webSite+=".bgimg {";
        webSite+="background-image: url('https://www.uxfree.com/wp-content/uploads/2016/08/macro-plex-4k-motion-background-loop-768x432.jpg');";
        webSite+="min-height: 100%;";
        webSite+="background-position: center;";
        webSite+="background-size: cover;";
    webSite+="}";
    webSite+="</style>";
    
    webSite+=javaScript;
    
    webSite+="<BODY onload='process()'>\n";
    webSite+="<div class=\"bgimg w3-display-container w3-animate-opacity w3-text-white\">";
    webSite+="<div class=\"w3-display-topleft w3-padding-large w3-xlarge\">";
    webSite+="Tsafe";
    webSite+="</div>";
    webSite+="<div class=\"w3-display-middle\">";
      webSite+="<h1 class=\"w3-jumbo w3-animate-top\">Trailer Armed</h1>";
      webSite+="<hr class=\"w3-border-grey\" style=\"margin:auto;width:40%\">";
      webSite+="<p class=\"w3-large w3-center\">System Voltage: <A id='Voltage'></A></p>";
      webSite+="<p class=\"w3-large w3-center\">Zone 1: <A id='Sensor1'></A></p>";
      webSite+="<p class=\"w3-large w3-center\">Zone 2: <A id='Sensor2'></A></p>";
      webSite+="<p class=\"w3-large w3-center\"><A id='Accelerometer'></A></p>";
      webSite+="<p class=\"w3-large w3-center\">App Alerts: <A id='AppAlert'></A></p>";
      webSite+="<p class=\"w3-large w3-center\">Email Alerts: <A id='EmailAlert'></A></p>";
    webSite+="</div>";
    webSite+="<div class=\"w3-display-bottomright w3-padding-large\">";
    webSite+="\"<a href=\"/settingsPage\">Settings</a>";
    webSite+=" </div>";
    webSite+="</div>";

    webSite+="</BODY>\n";
  webSite+="</HTML>\n";
}

void buildPage2(){
  buildJavascript();
  webSite="<!DOCTYPE HTML>\n";
  webSite+="<meta charset=\"UTF-8\">";
  webSite+="<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  webSite+="<link rel=\"stylesheet\" href=\"https://www.w3schools.com/w3css/4/w3.css\">";
  webSite+="<link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/css?family=Raleway\">";
  
  webSite+="<style>";
    webSite+="body,h1 {font-family: \"Raleway\", sans-serif}";
    webSite+="body, html {height: 100%}";
    webSite+=".bgimg {";
        webSite+="background-image: url('https://www.uxfree.com/wp-content/uploads/2016/08/macro-plex-4k-motion-background-loop-768x432.jpg');";
        webSite+="min-height: 100%;";
        webSite+="background-position: center;";
        webSite+="background-size: cover;";
    webSite+="}";

     webSite+=".switch {";
      webSite+="position: relative;";
      webSite+="display: inline-block;";
      webSite+="width: 60px;";
      webSite+="height: 34px;";
    webSite+="}";
    
    webSite+=".switch input {display:none;}";
    
    webSite+=".slider {";
      webSite+="position: absolute;";
      webSite+="cursor: pointer;";
      webSite+="top: 0;";
      webSite+="left: 0;";
      webSite+="right: 0;";
      webSite+="bottom: 0;";
      webSite+="background-color: #ccc;";
      webSite+="-webkit-transition: .4s;";
      webSite+="transition: .4s;";
    webSite+="}";
    
    webSite+=".slider:before {";
      webSite+="position: absolute;";
      webSite+="content: \"\";";
     webSite+=" height: 26px;";
      webSite+="width: 26px;";
      webSite+="left: 4px;";
      webSite+="bottom: 4px;";
      webSite+="background-color: white;";
      webSite+="-webkit-transition: .4s;";
      webSite+="transition: .4s;";
    webSite+="}";
    
    webSite+="input:checked + .slider {";
      webSite+="background-color: #2196F3;";
    webSite+="}";
    
    webSite+="input:focus + .slider {";
      webSite+="box-shadow: 0 0 1px #2196F3;";
    webSite+="}";
    
    webSite+="input:checked + .slider:before {";
      webSite+="-webkit-transform: translateX(26px);";
      webSite+="-ms-transform: translateX(26px);";
      webSite+="transform: translateX(26px);";
    webSite+="}";
    
    /* Rounded sliders */
    webSite+=".slider.round {";
      webSite+="border-radius: 34px;";
    webSite+="}";
    
    webSite+=".slider.round:before {";
      webSite+="border-radius: 50%;";
    webSite+="}";

//    webSite+="meter {";
//      webSite+="width: 126px;";
//      webSite+="height: 20px;";
//    webSite+="}";


    webSite+="meter {";
      webSite+="width: 125px;";
      webSite+="height: 20px;";
      webSite+="-webkit-appearance: none; /* Reset appearance */";
      webSite+="border: 1px solid #ccc;";
      webSite+="border-radius: 3px;";
    webSite+="}";

    
    webSite+="meter::-webkit-meter-bar {";
      webSite+="background: none; /* Required to get rid of the default background property */";
      webSite+="background-color: white;";
      webSite+="box-shadow: 0 5px 5px -5px #333 inset;";
    webSite+="}";
    
    webSite+="</style>";
    
    webSite+=javaScript;
    
    webSite+="<BODY onload='process()'>\n";
    webSite+="<div class=\"bgimg w3-display-container w3-animate-opacity w3-text-white\">";
    //webSite+="<div class=\"w3-display-topleft w3-padding-large w3-xlarge\">";
    //webSite+="Tsafe";
    //webSite+="</div>";
    webSite+="<div class=\"w3-display-middle\">";
      webSite+="<h1 class=\"w3-jumbo w3-animate-top\">Settings</h1>";
      webSite+="<hr class=\"w3-border-grey\" style=\"margin:auto;width:40%\">";
      webSite+="<table>";
      webSite+="<form action=\"/action_page.php\">"; //Change save

      webSite+="<tr><td><label for=\"Wifi_Strength\">Wifi Signal Strength:"+ Wifi_Strength +"db </label>";               
      webSite+="<td><meter max=-30 min=-80 value=\""+ Wifi_Strength +"\"></meter></td>";
      
      
      webSite+="<tr><td><label for=\"txtIp_address\">Ip Address:</label>";               
      webSite+="<td><input type=\"text\" value=\""+ UnitIP +"\" id=\"Ip_address\" size=\"15\">";

      webSite+="<tr><td><label for=\"txtWeb_Port\">Port:</label>";               
      webSite+="<td><input type=\"text\" value=\""+ String(Web_Port) +"\" id=\"Web_Port\" size=\"15\">";

      webSite+="<tr><td><label for=\"Wifi_SSID\">Wifi SSID:</label>";               
      webSite+="<td><input type=\"text\" value=\""+ String(ssid) +"\" id=\"Wifi_SSID\" size=\"15\">";

      webSite+="<tr><td><label for=\"txtWifi_Password\">Wifi Password:</label>";               
      webSite+="<td><input type=\"text\" value=\""+ String(Wifi_Password) +"\" id=\"Wifi_Password\" size=\"15\">";

      webSite+="<tr><td><label for=\"txtUser_Name\">Login Username:</label>";               
      webSite+="<td><input type=\"text\" value=\""+ String(www_username) +"\" id=\"User_Name\" size=\"15\">";

      webSite+="<tr><td><label for=\"txtUser_Password\">Login Password:</label>";               
      webSite+="<td><input type=\"text\" value=\""+ String(www_password) +"\" id=\"User_Password\" size=\"15\">";

      webSite+="<tr><td><label for=\"txtPIR1_Active\">PIR1 Active:</label>";               
      webSite+="<td><label class=\"switch\"><input type=\"checkbox\"id=\"Check_PIR1\">";
        webSite+="<span class=\"slider round\"></span>";
      webSite+="</label></td>";

      webSite+="<tr><td><label for=\"txtPIR2_Active\">PIR2 Active:</label>";              
      webSite+="<td><label class=\"switch\"><input type=\"checkbox\"id=\"Check_PIR2\">";
        webSite+="<span class=\"slider round\"></span>";
      webSite+="</label></td>";

      webSite+="<tr><td><label for=\"PIR_Sensitivity\">PIR Sensitivity:</label>";               
      webSite+="<td><label class=\"switch\"><input type=\"checkbox\"id=\"Check_PIRSensative\">";
        webSite+="<span class=\"slider round\"></span>";
      webSite+="</label></td>";

      webSite+="<tr><td><label for=\"Accelerometer_Active\">Accelerometer Active:</label>";               
      webSite+="<td><label class=\"switch\"><input type=\"checkbox\"id=\"Check_Accel\">";
        webSite+="<span class=\"slider round\"></span>";
      webSite+="</label></td>";
      
      webSite+="<tr><td><label for=\"Accelerometer_Sensitivity\">Accelerometer Sensitivity:</label>";               
      webSite+="<td><label class=\"switch\"><input type=\"checkbox\"id=\"Check_AccelSensative\">";
        webSite+="<span class=\"slider round\"></span>";
      webSite+="</label></td>";

      webSite+="<tr><td><label for=\"Reset_Default\">Reset factory Default</label>";               
      webSite+="<td></td>";

      webSite+="<tr><td><label for=\"Blank_Space\"></label>";               
      webSite+="<td></td>";
      
      webSite+="<tr><td><label for=\"Save_btn\"></label><input type=\"submit\" value=\"Save\";>";               
      webSite+="<td></td>";
      webSite+="</form>";
      webSite+="</table>";
      
    webSite+="</div>";
    webSite+="<div class=\"w3-display-bottomright w3-padding-large\">";
    webSite+="\"<a href=\"/\">Home</a>";
    webSite+=" </div>";
    webSite+="</div>";
    webSite+="</BODY>\n";
  webSite+="</HTML>\n";
}




void buildJavascript(){
  javaScript="<SCRIPT>\n";
  javaScript+="var xmlHttp=createXmlHttpObject();\n";

  javaScript+="function createXmlHttpObject(){\n";
  javaScript+=" if(window.XMLHttpRequest){\n";
  javaScript+="    xmlHttp=new XMLHttpRequest();\n";
  javaScript+=" }else{\n";
  javaScript+="    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
  javaScript+=" }\n";
  javaScript+=" return xmlHttp;\n";
  javaScript+="}\n";

  javaScript+="function process(){\n";
  javaScript+=" if(xmlHttp.readyState==0 || xmlHttp.readyState==4){\n";
  javaScript+="   xmlHttp.open('PUT','xml',true);\n";
  javaScript+="   xmlHttp.onreadystatechange=handleServerResponse;\n"; // no brackets?????
  javaScript+="   xmlHttp.send(null);\n";
  javaScript+=" }\n";
  javaScript+=" setTimeout('process()',1000);\n";
  javaScript+="}\n";
  
  javaScript+="function handleServerResponse(){\n";
  javaScript+=" if(xmlHttp.readyState==4 && xmlHttp.status==200){\n";
  javaScript+="   xmlResponse=xmlHttp.responseXML;\n";
  javaScript+="   xmldoc = xmlResponse.getElementsByTagName('Vresponse');\n";
  javaScript+="   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript+="   document.getElementById('Voltage').innerHTML=message;\n";

  javaScript+="   xmldoc = xmlResponse.getElementsByTagName('S1response');\n";
  javaScript+="   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript+="   document.getElementById('Sensor1').innerHTML=message;\n";

  javaScript+="   xmldoc = xmlResponse.getElementsByTagName('S2response');\n";
  javaScript+="   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript+="   document.getElementById('Sensor2').innerHTML=message;\n";

  javaScript+="   xmldoc = xmlResponse.getElementsByTagName('ACCresponse');\n";
  javaScript+="   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript+="   document.getElementById('Accelerometer').innerHTML=message;\n";

  javaScript+="   xmldoc = xmlResponse.getElementsByTagName('AAresponse');\n";
  javaScript+="   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript+="   document.getElementById('AppAlert').innerHTML=message;\n";

  javaScript+="   xmldoc = xmlResponse.getElementsByTagName('EAresponse');\n";
  javaScript+="   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript+="   document.getElementById('EmailAlert').innerHTML=message;\n";
  
  javaScript+=" }\n";
  javaScript+="}\n";
  javaScript+="</SCRIPT>\n";
}

void buildXML(){
  XML="<?xml version='1.0'?>";
  XML+="<response>";
  XML+="<Vresponse>";
  XML+=XML_Volt();
  XML+="</Vresponse>";

  XML+="<S1response>";
  XML+=XML_Sensor1();
  XML+="</S1response>";

  XML+="<S2response>";
  XML+=XML_Sensor2();
  XML+="</S2response>";

  XML+="<ACCresponse>";
  XML+=XML_Accel();
  XML+="</ACCresponse>";

  XML+="<AAresponse>";
  XML+=XML_AppAlert();
  XML+="</AAresponse>";


  XML+="<EAresponse>";
  XML+=XML_EmailAlert();
  XML+="</EAresponse>";

  
  XML+="</response>";
}
String XML_Volt(){
    MV1();
    String Stringout = "";
    Stringout = voltageString;
    return Stringout;
    
}
String XML_Sensor1(){
    PIRs();
    String Stringout = "";
    Stringout +=  Zone1 +"\n";
    return Stringout;
    
}
String XML_Sensor2(){
    //PIRs();
    String Stringout = "";
    Stringout +=  Zone2 +"\n";
    return Stringout;
    
}
String XML_Accel(){
    T_accel();
    String Stringout = "";
    Stringout +=  AccelString +"\n";
    return Stringout;
    
}
String XML_AppAlert(){
    //Appalert();
    String Stringout = "";
    Stringout +=  "Deactivated\n";
    return Stringout;
    
}
String XML_EmailAlert(){
    //EmailAlert();
    String Stringout = "";
    Stringout +=  "Deactivated\n";
    return Stringout;
    
}


void handleWebsite(){
  buildWebsite();
  server.send(200,"text/html",webSite);
}

void handlePage2(){
  buildPage2();
  server.send(200,"text/html",webSite);
}

void handleXML(){
  buildXML();
  server.send(200,"text/xml",XML);
}

void setup() {
  ///////-----PIR------///////
  pinMode(led_ACC, OUTPUT);      // declare LED + as output
  pinMode(led_PIR, OUTPUT);      // declare LED _ as output
  pinMode(Sens1, INPUT);     // declare PIR 1 as input
  pinMode(Sens2, INPUT);     // declare PIR 2 as input
  digitalWrite(led_PIR, LOW);  
  digitalWrite(led_ACC, LOW);
  ///////-----___------///////
  ///////-----ACCEL------///////
  //Wire.begin(int sda, int scl);
  Wire.begin(D3, D4);
    /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("No ADXL345 detected ... Check connections");
    //while(1);
  }

  /* Set the range */
   accel.setRange(ADXL345_RANGE_2_G);
   //Serial.println("<T_Accel_Init>");
  ///////-----___------///////
  Serial.begin(115200);  
  WiFi.begin(ssid,Wifi_Password);
  while(WiFi.status()!=WL_CONNECTED)delay(500);
  WiFi.mode(WIFI_STA);
  
  Serial.println("\n\n<Device Init>");
  
  Serial.print("Connected to ");
  Serial.println(ssid);
  
  Serial.print("Unit IP address: ");
  UnitIP = WiFi.localIP().toString();
  Serial.println(UnitIP);
  
  Serial.print("Signal Strength ");
  Wifi_Strength = String(WiFi.RSSI());
  Serial.println(WiFi.RSSI());
  
  //server.on("/",handleWebsite);
  server.on("/", [](){
  if(!server.authenticate(www_username, www_password))
    return server.requestAuthentication();
    handleWebsite();
  });
  server.on("/xml",handleXML);
  server.on("/settingsPage", []() { //Define the handling function for the CSS path
    //server.send(200, "text/html", settingsPage);
    handlePage2();
  });
  server.begin();  
}

void MV1() {
if(VoltS_printcycle >= VoltS_printlimit){
  voltage = analogRead(A0);
  voltage = (voltage / (1024))*4.95;
  voltage = voltage / (denominator * calibrateFactor);
  //Serial.print("Volts: ");
  //Serial.println(voltage);
  voltageString = String(voltage);
  VoltS_printcycle = 0;
  }
  //Delay to make serial out readable
  VoltS_printcycle ++;
}

void PIRs(){
  val1 = digitalRead(Sens1);  // read input value
  val2 = digitalRead(Sens2);  // read input value
  //////----------PR1---------------//////
  if (val1 == HIGH) {            // check if the input 1 is HIGH
    //digitalWrite(led_ACC, HIGH);  // turn LED ON
    //digitalWrite(led_PIR, HIGH);  // turn LED ON
    if (pirState1 == LOW) {
      // we have just turned on
      Serial.println("PIR 1 Motion detected!");
      digitalWrite(led_PIR, HIGH);  // turn LED ON
      Zone1 = "Violated";
      // We only want to print on the output change, not state
      pirState1 = HIGH;
    }
  } else {
    //digitalWrite(led_ACC, LOW); // turn LED OFF
    //digitalWrite(led_PIR, LOW); 
    if (pirState1 == HIGH){
      // we have just turned of
      Serial.println("PIR 1 Motion ended!");
      digitalWrite(led_PIR, LOW);
      Zone1 = "Clear";
      // We only want to print on the output change, not state
      pirState1 = LOW;
    }
  }
  //////----------PR2---------------//////
    if (val2 == HIGH) {            // check if the input 2 is HIGH
    //digitalWrite(led_ACC, HIGH);  // turn LED ON
   // digitalWrite(led_PIR, HIGH);
    if (pirState2 == LOW) {
      // we have just turned on
      Serial.println("PIR 2 Motion detected!");
      digitalWrite(led_PIR, HIGH);
      Zone2 = "Violated";
      // We only want to print on the output change, not state
      pirState2 = HIGH;
    }
  } else {
    //digitalWrite(led_PIR, LOW); // turn LED OFF
    if (pirState2 == HIGH){
      // we have just turned of
      Serial.println("PIR 2 Motion ended!");
      digitalWrite(led_PIR, LOW);
      Zone2 = "Clear";
      // We only want to print on the output change, not state
      pirState2 = LOW;
    }
  }
}

void T_accel() {
  sensors_event_t event; 
  accel.getEvent(&event);
  Tx = Tx + float(event.acceleration.x);
  Ty = Ty + float(event.acceleration.y);
  Tz = Tz + float(event.acceleration.z);
  
  //Output to serial
  if(printcycle_ACC >= printlimit_ACC){
      Tx = Tx/printcycle_ACC;
      Ty = Ty/printcycle_ACC;
      Tz = Tz/printcycle_ACC;
      
      float Txdif = Tx-TxPast;
      float Tydif = Ty-TyPast;
      float Tzdif = Tz-TzPast;
      
      //Serial.println(Txdif);
      //Serial.println(Tydif);
      //Serial.println(Tzdif);
      digitalWrite(led_ACC, LOW);
      AccelString = "Device is Stationary";
      
      if (Txdif > ps_Dif_Factor || Txdif < ng_Dif_Factor) {
        Serial.println("Warning X-Motion");
        digitalWrite(led_ACC, HIGH);
        AccelString = "Device is in Motion";
        }
      if (Tydif > ps_Dif_Factor || Tydif < ng_Dif_Factor) {
        Serial.println("Warning Y-Motion");
        digitalWrite(led_ACC, HIGH);
        AccelString = "Device is in Motion";
        }
      if (Tzdif > ps_Dif_Factor || Tzdif < ng_Dif_Factor) {
        Serial.println("Warning Z-Motion");
        digitalWrite(led_ACC, HIGH);
        AccelString = "Device is in Motion";
        }
      

      TxPast = Tx;
      TyPast = Ty;
      TzPast = Tz;
      
      Tx=0;
      Ty=0;
      Tz=0;
      printcycle_ACC = 0;
  }
  //Delay to make serial out readable
  printcycle_ACC ++;
 

  //delay(1);
}

void OnceHour(){
  if(Hour_printcycle >= Hour_printlimit){
    Wifi_Strength = String(WiFi.RSSI());
    Serial.println(WiFi.RSSI());
    Hour_printcycle =0;
  }
  Hour_printcycle ++;
  
}

void loop() {
   server.handleClient();
   MV1();
   PIRs();
   T_accel(); 
   OnceHour();

}
