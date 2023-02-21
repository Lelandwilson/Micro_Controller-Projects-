#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#include <NTPClient.h>
#include <FS.h>
#include "SPIFFS.h"


/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
//#define FORMAT_SPIFFS_IF_FAILED true

String Load_TIME_settings = "";
String Load_WIFI_settings = "";
String Load_USER_settings = "";
String Load_Settings = "";

String Default_Ontime = "19:1";
String Default_Offtime = "23:30";
String Ontime = "";
String Offtime = "";


const char* default_ssidX = "SSID_HERE";
const char* default_Wifi_PassX = "WIFI_PASS_HERE";

const char* default_userX = "admin";
const char* default_passX = "esp8266";

char ssid[25];
char Wifi_Password[25];

char www_username[25];
char www_password[25];

int Web_Port = 8088;
WebServer server(Web_Port);

String Version = "1.1";
String Hardware = "ESP32";

volatile bool statusLed = false;
volatile uint32_t lastMillis = 0;

#define TIMER_INTERVAL_MS       1000

// Init ESP8266 timer 0
//ESP8266Timer ITimer;

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete


//String Ontime = "19:1";
//String Offtime = "23:30";
bool R1status = false;

String currentTime = "";
String CompareTime ="";
String dayX = "";
int hourX = 0;
int minuteX = 0;
int secondX = 0;
const int daylightSavings = 3600;
const long utcOffsetInSeconds = 36000 + daylightSavings;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
bool NTPflag = false;

String webSite,javaScript,XML;
String UnitIP = "";
String Wifi_Strength = "";

int Hour_printcycle = 0;
int Hour_printlimit = 5000000;
int RELAY1 = 22;  
int cycleCount = 0;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "oceania.pool.ntp.org", utcOffsetInSeconds);
//      ---       //

//       Timer setup      //

hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;

void IRAM_ATTR onTimer(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter++;
  lastIsrAt = millis();
  portEXIT_CRITICAL_ISR(&timerMux);
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
  // It is safe to use digitalRead/Write here if you want to toggle an output
}



void buildWebsite() {
  buildJavascript();
  webSite = "<!DOCTYPE HTML>\n";
  webSite += "<meta charset=\"UTF-8\">";
  webSite += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  webSite += "<link rel=\"stylesheet\" href=\"https://www.w3schools.com/w3css/4/w3.css\">";
  webSite += "<link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/css?family=Raleway\">";
  webSite += "<script src=\"http://ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js\"></script>";

  webSite += "<link href=\"https://gitcdn.github.io/bootstrap-toggle/2.2.2/css/bootstrap-toggle.min.css\" rel=\"stylesheet\">";
  webSite += "<script src=\"https://gitcdn.github.io/bootstrap-toggle/2.2.2/js/bootstrap-toggle.min.js\"></script>";

  webSite += "<style>";
  webSite += "body,h1 {font-family: \"Raleway\", sans-serif}";
  webSite += "body, html {height: 100%}";
  webSite += ".bgimg {";
  webSite += "background-image: url('https://starwarsblog.starwars.com/wp-content/uploads/2020/04/star-wars-backgrounds-04.jpg');";
  webSite += "min-height: 100%;";
  webSite += "background-position: center;";
  webSite += "background-size: cover;";
  webSite += "}";


  webSite += "meter {";
  webSite += "width: 125px;";
  webSite += "height: 20px;";
  webSite += "-webkit-appearance: none; /* Reset appearance */";
  webSite += "border: 1px solid #ccc;";
  webSite += "border-radius: 3px;";
  webSite += "}";


  webSite += "meter::-webkit-meter-bar {";
  webSite += "background: none; /* Required to get rid of the default background property */";
  webSite += "background-color: white;";
  webSite += "box-shadow: 0 5px 5px -5px #333 inset;";
  webSite += "}";

  webSite += "</style>";

  webSite += javaScript;

  webSite += "<BODY onload='process()'>\n";
  webSite += "<div class=\"bgimg w3-display-container w3-animate-opacity w3-text-white\">";
  webSite += "    <div class=\"w3-display-topleft w3-padding-large w3-xlarge\">";
  webSite += "    Control Centre";
  webSite += "</div>";

  webSite += "<div class=\"w3-display-topright w3-padding-large\">";
  webSite += "    <p class=\"text-muted\"><A id='T_id'></A></p>"; //Place holder for current time
  webSite += " </div>";

  webSite += "<div class=\"w3-display-middle\">";
  //webSite+="<h1 class=\"w3-jumbo w3-animate-top\">Control Center</h1>";
  webSite += "    <hr class=\"w3-border-grey\" style=\"margin:auto;width:20%\">";
  webSite += "    <table>";
//  webSite+="<form action=\"post\">"; //Change save   ///////

  webSite += "    <tr><td><label for=\"Wifi_Strength\" id=\"sigStrength_id_1\">Wifi Signal Strength:" + Wifi_Strength + "db </label>";
  webSite += "    <td><meter max=100 min=0 value=\"" + Wifi_Strength + " \"id=\"sigStrength_id_2\"></meter></td>";

  webSite += "    <tr><td><label for=\"Blank_Space\"></label>";
  webSite += "    <td></td>";
  webSite += "    <tr><td><label for=\"Blank_Space\"></label>";
  webSite += "    <td></td>";


  webSite += "    <tr><td><label for=\"txt_R1\">Festoon Lights:</label>";
  //webSite+="</td>";
  webSite +=    "<td><input id=\"toggle-one\" type=\"checkbox\" data-toggle=\"toggle\" data-onstyle=\"success\" data-offstyle=\"danger\">";
  webSite += "    </label></td>";

  webSite+="<form action=\"/save\">"; //Change save   ///////
  webSite += "    <tr><td><label for=\"txtOntime\" size=\"15\">On Time:</label>";
  webSite += "        <td><input type=\"text\" value=\"" + String(Ontime) + "\" id=\"Ontime\" name=\"Ontime\" size=\"15\">";
  webSite += "    </label></td>";

  webSite += "    <tr><td><label for=\"txtOfftime\" size=\"15\">Off Time:</label>";
  webSite += "        <td><input type=\"text\" value=\"" + String(Offtime) + "\" id=\"Offtime\" name=\"Offtime\" size=\"15\">";
  webSite += "    </label></td>";


  webSite += "    <tr><td><label for=\"Blank_Space\"></label>";
  webSite += "    <td></td>";

  webSite += "    <tr><td><label for=\"Blank_Space\"></label>";
  webSite += "    <td></td>";
  
  webSite+="      <tr><td><label for=\"Blank_Space\"></label>";               
  webSite+="      <td></td>";
  
  webSite+="      <tr><td><label for=\"Save_btn\"></label><input type=\"submit\" value=\"Save\";>";               
  webSite+="      <td></td>";
  webSite+="      </form>";  //////////////
  webSite += "    </table>";

  webSite += "    </div>";
  webSite += "    <div class=\"w3-display-bottomright w3-padding-large\">";
  webSite +=        "\"<a href=\"/settingsPage\">Settings</a>";
  webSite += "        </div>";
  webSite += "    </div>";
  webSite += "</BODY>\n";
  webSite += "</HTML>\n";
}

void buildJavascript() {
  javaScript = "<SCRIPT>\n";
  //javaScript+="$('#toggle-demo').bootstrapToggle();\n";
  javaScript += "var xmlCount = 0;\n";
  //  javaScript+="$('#toggle-one').bootstrapToggle()\n";
  javaScript += "var xmlHttp=createXmlHttpObject();\n";

  javaScript += "function createXmlHttpObject(){\n";
  javaScript += " if(window.XMLHttpRequest){\n";
  javaScript += "    xmlHttp=new XMLHttpRequest();\n";
  javaScript += " }else{\n";
  javaScript += "    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
  javaScript += " }\n";
  javaScript += " return xmlHttp;\n";
  javaScript += "}\n";

  javaScript += "function process(){\n";
  javaScript += " if(xmlHttp.readyState==0 || xmlHttp.readyState==4){\n";
  javaScript += "   xmlHttp.open('PUT','xml',true);\n";
  javaScript += "   xmlHttp.onreadystatechange=handleServerResponse;\n"; // no brackets?????
  javaScript += "   xmlHttp.send(null);\n";
  //
  javaScript += "  xmlCount = xmlCount+1;\n";
  //  javaScript+="  console.log(xmlCount);\n";

  javaScript += " }\n";
  javaScript += " setTimeout('process()',1000);\n";
  javaScript += "}\n";

  javaScript += "function handleServerResponse(){\n";
  javaScript += " if(xmlHttp.readyState==4 && xmlHttp.status==200){\n";
  javaScript += "   xmlResponse=xmlHttp.responseXML;\n";

  javaScript += "   xmldoc = xmlResponse.getElementsByTagName('T_idResponse');\n";
  javaScript += "   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript += "   document.getElementById('T_id').innerHTML=message;\n";

  javaScript += "   xmldoc = xmlResponse.getElementsByTagName('sigStrengthResponse');\n";
  javaScript += "   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript += "   document.getElementById('sigStrength_id_1').innerHTML='Wifi Signal Strength:'+ message;\n";
  javaScript += "   document.getElementById('sigStrength_id_2').innerHTML=message;\n";

  javaScript += "   xmldoc = xmlResponse.getElementsByTagName('r1Response');\n";
  javaScript += "   message = xmldoc[0].firstChild.nodeValue;\n";


  javaScript += "   if (xmlCount == 1) {\n";
  javaScript += "       if (message== 'checked'){\n";
  //javaScript+="     document.getElementById('toggle-one').innerHTML=message;\n";
  javaScript += "          $('#toggle-one').bootstrapToggle('on')\n";
  javaScript += "       }\n";
  javaScript += "   }\n";
  javaScript += "   else {\n";
  javaScript += "       if (xmlCount == 1) {\n";
  javaScript += "           $('#toggle-one').bootstrapToggle('off')\n";
  javaScript += "       }\n";
  javaScript += "   }\n";




  javaScript += "   if (xmlCount!= xmlCount) {\n";
  javaScript += "       if (message== 'checked'){\n";
  //javaScript+="     document.getElementById('toggle-one').innerHTML=message;\n";
  javaScript += "          $('#toggle-one').bootstrapToggle('on')\n";
  javaScript += "       }\n";
  javaScript += "   }\n";
  javaScript += "   else {\n";
  javaScript += "       if (xmlCount!= xmlCount) {\n";
  javaScript += "           $('#toggle-one').bootstrapToggle('off')\n";
  javaScript += "       }\n";
  javaScript += "   }\n";



  javaScript += " }\n";
  javaScript += "}\n";



  javaScript += "$(document).ready(function() {\n";

  javaScript += "$( \"#toggle-one\" ).on('change', function() {\n";
  //javaScript+="    if (xmlCount!= xmlCount) {\n";
  javaScript += "        if ($(this).is(':checked')) {\n";
  javaScript += "            switchStatus = $(this).is(':checked');\n";
  //javaScript+="            console.log('#toggle-one ON');\n";
  javaScript += "            var updatedData = $.post('/Ron');\n";

  javaScript += "        }\n";
  //javaScript+="    }\n";

  javaScript += "    else {\n";
  //javaScript+="       if (xmlCount!= xmlCount) {\n";
  javaScript += "           switchStatus = $(this).is(':checked');\n";
  //javaScript+="         console.log('#toggle-one OFF');\n";
  javaScript += "           var updatedData = $.post('/Roff');\n";
  //javaScript+="        }\n";
  javaScript += "    }\n";
  javaScript += "});\n";
  javaScript += "});\n";


  //javaScript+="$(document).ready(function() {;\n";
  //javaScript+="    $(\"#B1\").click(function(){;\n";
  //javaScript+="        alert(\"button\");;\n";
  //javaScript+="    });\n";
  //javaScript+="});\n";



  javaScript += "</SCRIPT>\n";
}

void buildXML() {
  XML = "<?xml version='1.0'?>";
  XML += "<response>";
  XML += "<T_idResponse>";
  //XML+= String(currentTime);
  XML += XML_NTP();
  XML += "</T_idResponse>";


  XML += "<sigStrengthResponse>";
  XML += XML_sigStrength();
  XML += "</sigStrengthResponse>";

  XML += "<r1Response>";
  XML += XML_R1();
  XML += "</r1Response>";

  XML += "</response>";
}

String XML_R1() {
  String Stringout = "";
  if (R1status == true) {
    Stringout = "checked";
  }
  else {
    Stringout = "unchecked";
  }
  //Stringout = String(RELAY1);
  return Stringout;
}

String XML_NTP() {
  //getNTPtime();           ///////////////fix this call to NTP()
  String Stringout = "";
  Stringout = CompareTime;
  return Stringout;
}

String XML_sigStrength() {
  int WifiSig = WiFi.RSSI();
  //Wifi_Strength = String(map(WifiSig,-100,-34,0,100));
  WifiSig = map(WifiSig, -100, -40, 0, 100);
  WifiSig = 2 * WifiSig;
  Wifi_Strength = String(WifiSig);
  String Stringout = Wifi_Strength;
  //String Stringout = String(WiFi.RSSI());
  return Stringout;
}

void handleWebsite() {
  buildWebsite();
  server.send(200, "text/html", webSite);
}

void handleRon() {
  Serial.print("Relay ON [");
  Serial.print(String(CompareTime));
  Serial.println("]");
  digitalWrite(RELAY1, HIGH);
  R1status = true;
}

void handleRoff() {
  Serial.print("Relay OFF [");
  Serial.print(String(CompareTime));
  Serial.println("]");
  digitalWrite(RELAY1, LOW);
  R1status = false;

}

void handlesave() {
 Ontime = server.arg("Ontime"); 
 Offtime = server.arg("Offtime"); 

 Serial.print("New Ontime:");
 Serial.println(Ontime);
 Serial.print("New Offtime:");
 Serial.println(Offtime);

 File Time_file = SPIFFS.open("/TIMEsettings.txt", "w");
 Time_file.println("ON=" + String(Default_Ontime));
 Time_file.println("OFF=" + String(Default_Offtime));
 Time_file.close();
 Serial.println("Save File Updated");
 server.send(200, "text/html", webSite);

}

void handleXML() {
  buildXML();
  server.send(200, "text/xml", XML); //send XML to webserver
}


//no need authentification
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
    Serial.begin(115200); 
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  //SPIFFS.format();  /////////////////REQUIRED FOR FIRST TIME FORMATTING OF NEW UNIT


  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    Serial.println("Attempting to Format SPIFFS");
    SPIFFS.format();  /////////////////REQUIRED FOR FIRST TIME FORMATTING OF NEW UNIT

    delay(5000);
    return;
  }
  Serial.println("\n<Loading files from memory>");

// Wifi Setttings load //
  File Wifi_file = SPIFFS.open("/WIFIsettings.txt", FILE_READ);
  if (!Wifi_file) {
    Serial.println("/WIFIsettings.txt not present");
    File Wifi_file = SPIFFS.open("/WIFIsettings.txt", FILE_WRITE);
    Wifi_file.println("SSID=" + String(default_ssidX));
    Wifi_file.println("PASS=" + String(default_Wifi_PassX));
    Wifi_file.close();
    delay(10);
  }
  while (Wifi_file.available()) {Load_WIFI_settings += char(Wifi_file.read());}
    if (Load_WIFI_settings.length()<5){
    Serial.println("Load_WIFI_settings Error, file too short. Using Default");
    Wifi_file.close();
    SPIFFS.remove("/WIFIsettings.txt");
    File Wifi_file = SPIFFS.open("/WIFIsettings.txt", FILE_WRITE);
    Wifi_file.println("SSID=" + String(default_ssidX));
    Wifi_file.println("PASS=" + String(default_Wifi_PassX));
    Wifi_file.close();
    
    delay(10);
    Wifi_file = SPIFFS.open("/WIFIsettings.txt", FILE_READ);
    while (Wifi_file.available()) {Load_WIFI_settings += char(Wifi_file.read());}

    }
  
  Wifi_file.close();

  ///////// User settings load //
  File User_file = SPIFFS.open("/USERsettings.txt", FILE_READ);
  if (!User_file) {
    File User_file = SPIFFS.open("/USERsettings.txt", FILE_WRITE);
    Serial.println("/USERsettings.txt not present");
    User_file.println("USER=" + String(default_userX));
    User_file.println("PASS=" + String(default_passX));
    User_file.close();
    delay(10);
  }
  while (User_file.available()) {Load_USER_settings += char(User_file.read());}
  if (Load_USER_settings.length()<5){
    Serial.println("Load_USER_settings Error, file too short. Using Default");
    User_file.close();
    SPIFFS.remove("/USERsettings.txt");
    File User_file = SPIFFS.open("/USERsettings.txt", FILE_WRITE);
    User_file.println("USER=" + String(default_userX));
    User_file.println("PASS=" + String(default_passX));
    User_file.close();
    delay(10);
    User_file = SPIFFS.open("/USERsettings.txt", FILE_READ);
    while (User_file.available()) {Load_USER_settings += char(User_file.read());}
    }
    
  
  User_file.close();

  ///////// Time settings load //
  File Time_file = SPIFFS.open("/TIMEsettings.txt", FILE_READ);
  if (!Time_file) {
    File Time_file = SPIFFS.open("/TIMEsettings.txt", FILE_WRITE);
    Serial.println("/TIMEsettings.txt not present");
    Time_file.println("ON=" + String(Default_Ontime));
    Time_file.println("OFF=" + String(Default_Offtime));
    Time_file.close();
    delay(10);
  }
  while (Time_file.available()) {Load_TIME_settings += char(Time_file.read());}
  if (Load_TIME_settings.length()<5){
    Serial.println("Time_file Error, file too short. Using Default");
    Time_file.close();
    SPIFFS.remove("/TIMEsettings.txt");
    File Time_file = SPIFFS.open("/TIMEsettings.txt", FILE_WRITE);
    Time_file.println("ON=" + String(Default_Ontime));
    Time_file.println("OFF=" + String(Default_Offtime));
    Time_file.close();
    delay(10);
    Time_file = SPIFFS.open("/TIMEsettings.txt", FILE_READ);
    while (Time_file.available()) {Load_TIME_settings += char(Time_file.read());}
  }
  
  Time_file.close();


  Load_Settings += Load_WIFI_settings;
  Load_Settings += Load_USER_settings;

  Serial.println("Loaded:");
  Serial.print(Load_Settings);
  Serial.println("----------");

  int equal = Load_WIFI_settings.indexOf('=');
  int seperator = Load_WIFI_settings.indexOf('\n');
  String Xssid = Load_WIFI_settings.substring(equal+1, seperator);
  Xssid.replace("\n", "");
  Xssid.replace(" ", "");
  Xssid.toCharArray(ssid, Xssid.length());
  //Serial.println(ssid);
  
  equal = Load_WIFI_settings.indexOf('=', seperator);
  seperator = Load_WIFI_settings.indexOf('\n'+1,seperator);
  String XWifi_Password = Load_WIFI_settings.substring(equal+1, seperator);
  XWifi_Password.replace("\n", "");
  XWifi_Password.replace(" ", "");
  XWifi_Password.toCharArray(Wifi_Password, XWifi_Password.length());
  //Serial.println(XWifi_Password);

  equal = Load_USER_settings.indexOf('=');
  seperator = Load_USER_settings.indexOf('\n');
  String Xusername = Load_USER_settings.substring(equal+1, seperator);
  Xusername.replace("\n", "");
  Xusername.replace(" ", "");
  Xusername.toCharArray(www_username, Xusername.length());
  //Serial.println(www_username);

  equal = Load_USER_settings.indexOf('=', seperator);
  seperator = Load_USER_settings.indexOf(seperator);
  String Xpassword = Load_USER_settings.substring(equal+1, seperator);
  Xpassword.replace("\n", "");
  Xpassword.replace(" ", "");
  Xpassword.toCharArray(www_password, Xpassword.length());
  //Serial.println(www_password);

  equal = Load_TIME_settings.indexOf('=');
  seperator = Load_TIME_settings.indexOf('\n');
  String XonTime = Load_TIME_settings.substring(equal+1, seperator);
  XonTime.replace("\n", "");
  XonTime.replace(" ", "");
  Ontime = XonTime;
  //XonTime.toCharArray(Ontime, XonTime.length());

  equal = Load_TIME_settings.indexOf('=', seperator);
  seperator = Load_TIME_settings.indexOf(seperator);
  String Xofftime = Load_TIME_settings.substring(equal+1, seperator);
  Xofftime.replace("\n", "");
  Xofftime.replace(" ", "");
  Offtime = Xofftime;
  //XpassXOfftimeword.toCharArray(Offtime, XOfftime.length());

  Serial.println("Utilized:");
  Serial.println(ssid);
  Serial.println(Wifi_Password);
  Serial.println(www_username);
  Serial.println(www_password);
  Serial.println(Ontime);
  Serial.println(Offtime);
  Serial.println("----------");


//  Serial.println(Load_WIFI_settings);
//  Serial.println(Load_WIFI_settings.length());




  //     timer   //
    // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();

  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer = timerBegin(0, 80, true);
  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);
  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, 1000000, true);
  // Start an alarm
  timerAlarmEnable(timer);


  
  
  esp_err_t esp_wifi_set_max_tx_power(int8_t power);

    //Auto reconnect to WIFI settings
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  
    ///////-----Relay------///////
  pinMode(RELAY1, OUTPUT);      // declare LED + as output
  digitalWrite(RELAY1, LOW);  
  R1status = false;
  
  Serial.println("<Connecting to Wifi>"); 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, Wifi_Password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("\n<Device Init>");
  Serial.print("Version: ");
  Serial.println(Version);
  Serial.print("Hardware: ");
  Serial.println(Hardware);
  Serial.print("Connected to: ");
  Serial.println(ssid);
  Serial.print("Unit IP address: ");
  UnitIP = WiFi.localIP().toString();
  Serial.println(UnitIP);
  Serial.print("Signal Strength: ");
  int WifiSig = WiFi.RSSI();
  WifiSig = map(WifiSig,-100,-40,0,100);
  WifiSig = 2*WifiSig;
  Wifi_Strength = String(WifiSig);
  Serial.print(WifiSig);
  Serial.print("/");
  Serial.println(WiFi.RSSI());
  Serial.print("Webserver Active: ");
  Serial.print(UnitIP);
  Serial.print(":");
  Serial.println(String(Web_Port));
  

  
   server.on("/", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
      handleWebsite();
    });
    server.on("/xml",handleXML);
    server.on("/Ron",handleRon);
    server.on("/Roff",handleRoff);
    server.on("/save", handlesave);
    server.on("/settingsPage", []() { //Define the handling function for the CSS path
      //server.send(200, "text/html", settingsPage);
  
    });

  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);
  server.begin();
  Serial.println("HTTP server started");

  timeClient.begin();  //NTP instance
  getNTPtime();  // Get initial Time from NTP server
  
  Serial.print("Time stamp:");
  Serial.println(currentTime);
  Serial.println("'On' Time Setting: " + Ontime);
  Serial.println("'Off' Time Setting: " + Offtime);
  
  /// Initial check for current time versus relay set on/off time and set relay to suit ///
  CompareTime = String(hourX)+":"+String(minuteX);

 seperator = Ontime.indexOf(':');
 String on_hrX = Ontime.substring(0,seperator);
 int on_hr = on_hrX.toInt();
 String on_minX = Ontime.substring(seperator+1,Ontime.length());
 int on_min = on_minX.toInt();

 seperator = Offtime.indexOf(':');
 String off_hrX = Offtime.substring(0,seperator);
 int off_hr = off_hrX.toInt();
 String off_minX = Offtime.substring(seperator+1,Offtime.length());
 int off_min = off_minX.toInt();
  
  if(hourX >= on_hr && hourX <= off_hr){ // Meets hour condition
    if (hourX == on_hr){// Current hour and on hour are the same 
      if(minuteX>= on_min && minuteX<= off_min){//Meets on minute conditon within the hour
        handleRon();
        Serial.println("Startup_Timer Condition: Relay1 On"); 
        }
       else{
          handleRoff();
          Serial.println("Startup_Timer Condition: Relay1 Off"); 
        }
      }
        
     else{ //Not the same hour but ithin the hour conditions
      handleRon();
      Serial.println("Startup_Timer Condition: Relay1 On"); 
     }
  }
  else{
    handleRoff();
    Serial.println("Startup_Timer Condition: Relay1 Off"); 
    }
    
  
}

void loop(void) {
  server.handleClient();
  HWtimer_event();

   serialEvent();
   if (stringComplete) {
      Serial.println(inputString);
      // clear the string:
      inputString = "";
      stringComplete = false;
  }
}


void HWtimer_event(){
    // If Timer has fired
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE){
    uint32_t isrCount = 0, isrTime = 0;
    // Read the interrupt count and time
    portENTER_CRITICAL(&timerMux);
    isrCount = isrCounter;
    isrTime = lastIsrAt;
    portEXIT_CRITICAL(&timerMux);

    // ---------- Timed delayed fucntions go here -------------//
    secondX +=1 ;
    if (secondX>=59){
      secondX = 0;
      minuteX +=1;
      //String CompareTime = dayX+", "+String(hourX)+":"+String(minuteX)+":"+String(secondX);
      CompareTime = String(hourX)+":"+String(minuteX);
      //Serial.println(CompareTime);
  
      if(Ontime == CompareTime){
        handleRon();
        Serial.print("Timer setting Relay1 On ["); 
        Serial.println(CompareTime + "]"); 
        }
      if(Offtime == CompareTime){
        handleRoff();
        Serial.print("Timer setting Relay1 Off [");
        Serial.println(CompareTime + "]");
        }  
      
      if(minuteX>=59){
        minuteX = 0;
        hourX +=1;
        if((hourX==23) && (secondX<=55)){
          NTPflag = false;
          }
        if(hourX>23){
          if(NTPflag==false){
            getNTPtime();
            NTPflag = true;
            }
           if(hourX>24){
            cycleCount +=1;
            hourX = 0;
           }
          }
        }
      }
  }
}



void serialEvent() {
  if (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      serialMSG(inputString);
      
    }
  }
}

void serialMSG(String input){
  //String on_hrX = Ontime.substring(0,seperator);
  if(input.indexOf('admin_reboot')){
   Serial.println("Rebooting system"); 
   ESP.restart();
  }
  else if (input.indexOf('cycle_count')){
    Serial.print("Cycle count: ");
    Serial.println(String(cycleCount));
  }
  else if (input.indexOf('ip_address')){
    Serial.print("IP address: ");
    Serial.println(UnitIP);
    
  }
  else if (input.indexOf('web_server')){
    Serial.print("Web server: ");
    Serial.println(String(UnitIP)+ ":" + String(Web_Port));
  }
}


void getNTPtime(){
  timeClient.update();
  dayX = daysOfTheWeek[timeClient.getDay()];
  hourX = int(timeClient.getHours());
  minuteX = int(timeClient.getMinutes());
  secondX = int(timeClient.getSeconds());

  currentTime = "";
  currentTime += dayX;
  currentTime += ", ";
  currentTime += String(hourX);
  currentTime += ":";
  currentTime += String(minuteX);
  currentTime += ":";
  currentTime += String(secondX);
  //Serial.println(currentTime);

}
