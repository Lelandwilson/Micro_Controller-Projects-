#include <WiFiClient.h>
#include <WebServer.h>

#include <NTPClient.h>
#include <FS.h>
#include "SPIFFS.h"

#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

int LED_BUILTIN = 2;
int RELAY1 = 22;  
bool R1status = false;

String Load_WIFI_settings = "";
String Load_USER_settings = "";
String Load_Settings = "";

String webSite,javaScript,javaScript2,XML;
String UnitIP = "";
String Wifi_Strength = "";

const char* default_ssidX = "SSID_HERE";
const char* default_Wifi_PassX = "WIFI_PASS_HERE";

const char* default_userX = "admin";
const char* default_passX = "esp8266";

char ssid[25];
char Wifi_Password[25];

char www_username[25];
char www_password[25];

int Web_Port = 8088;
//WebServer server(Web_Port);
AsyncWebServer server(Web_Port);
String Version = "1.3";
String Hardware = "ESP32";

volatile bool statusLed = false;
volatile uint32_t lastMillis = 0;

unsigned long wifiCheck_previousMillis = 0;
unsigned long wifiCheck_interval = 30000;
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete




void setup() {
Serial.begin(115200); 
    pinMode(LED_BUILTIN, OUTPUT); // LEWd to indicate wifi connection
    digitalWrite(LED_BUILTIN, LOW);
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

  Serial.println("Utilized:");
  Serial.println(ssid);
  Serial.println(Wifi_Password);
  Serial.println(www_username);
  Serial.println(www_password);
  Serial.println("----------");

  esp_err_t esp_wifi_set_max_tx_power(int8_t power);

  //Auto reconnect to WIFI settings
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  
    ///////-----Relay------///////
  pinMode(RELAY1, OUTPUT);      // declare Relay as output
  digitalWrite(RELAY1, LOW);  
  R1status = false;
  
  Serial.println("<Connecting to Wifi>"); 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, Wifi_Password);
  Serial.println("");
  digitalWrite(LED_BUILTIN, LOW);
  // Wait for connection
  unsigned long wifiCheck_currentMillis = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    int ledstatus = digitalRead(LED_BUILTIN);
    if (ledstatus == 0){digitalWrite(LED_BUILTIN, HIGH);} //Blink wifi led
    else{digitalWrite(LED_BUILTIN, LOW);}
    if ((WiFi.status() != WL_CONNECTED) && (wifiCheck_currentMillis - wifiCheck_previousMillis >=wifiCheck_interval)) {
      Serial.println("Failed to connect to wifi, unit restarting");
      ESP.restart();
    }
  }
  digitalWrite(LED_BUILTIN, HIGH); // Turn led on to indicate Wifi connected
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

///////// OTA//////
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
  server.begin();
  ///////////

}

void loop() {
  ArduinoOTA.handle();
}

void serialMSG(String input){
  //String on_hrX = Ontime.substring(0,seperator);
  if(input.indexOf('admin_reboot')){
   Serial.println("Rebooting system"); 
   ESP.restart();
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
