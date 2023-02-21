
#include <ESP8266WiFi.h>
#include <Arduino.h>

// Set these to your desired softAP credentials. They are not configurable at runtime.
const char *ssid = "SSID HERE";
const char *password = "WIFI PASS HERE";
// Static IP details...
IPAddress ip(192, 168, 0, 135);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 0, 1);

boolean debug = false;  // true = more messages
//boolean debug = true;

// LED is needed for failure signalling
const short int BUILTIN_LED2 = 13;  //GPIO16 on NodeMCU (ESP-12)
const short int RELAY1 = 12;


unsigned long startTime = millis();

// provide text for the WiFi status
const char *str_status[]= {
  "WL_IDLE_STATUS",
  "WL_NO_SSID_AVAIL",
  "WL_SCAN_COMPLETED",
  "WL_CONNECTED",
  "WL_CONNECT_FAILED",
  "WL_CONNECTION_LOST",
  "WL_DISCONNECTED"
};

// provide text for the WiFi mode
const char *str_mode[]= { "WIFI_OFF", "WIFI_STA", "WIFI_AP", "WIFI_AP_STA" };
String commandString;
char charBuf[50];


//----------------------- WiFi handling
void connectWifi() {
  Serial.print("Connecting as wifi client to SSID: ");
  Serial.println(ssid);

  // use in case of mode problem
  WiFi.disconnect();
  // switch to Station mode
  if (WiFi.getMode() != WIFI_STA) {
    WiFi.mode(WIFI_STA);
  }

  WiFi.config(ip, gateway, subnet, dns); //If you need Internet Access You should Add DNS also...
  WiFi.begin ( ssid, password );

  if (debug ) WiFi.printDiag(Serial);

  // ... Give ESP 10 seconds to connect to station.
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  // Check connection
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected; IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.print("WiFi connect failed to ssid: ");
    Serial.println(ssid);
    Serial.print("WiFi password <");
    Serial.print(password);
    Serial.println(">");
    Serial.println("Check for wrong typing!");
  }
}  // connectWiFi()

void signalError() {  // loop endless with LED blinking in case of error
  while(1) {
      digitalWrite(BUILTIN_LED2, LOW);
      delay(500); // ms
      digitalWrite(BUILTIN_LED2, HIGH);
      delay(500); // ms
  }
}


// declare telnet server (do NOT put in setup())
WiFiServer telnetServer(23);
WiFiClient serverClient;


void setup() {
  delay(1000);
  
  pinMode(RELAY1, OUTPUT);      // declare Relay + as output
  digitalWrite(RELAY1, LOW);  
  
  Serial.begin(115200);
  delay(1000);
  Serial.println("Sync,Sync,Sync,Sync,Sync");
  delay(500);
  Serial.println();
  // signal start
  pinMode(BUILTIN_LED2, OUTPUT);
  digitalWrite(BUILTIN_LED2, LOW);
  delay(100); // ms
  digitalWrite(BUILTIN_LED2, HIGH);
  delay(300); // ms

  Serial.print("Chip ID: 0x");
  Serial.println(ESP.getChipId(), HEX);

  Serial.println ( "Connect to Router requested" );
  connectWifi();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi mode: ");
    Serial.println(str_mode[WiFi.getMode()]);
    Serial.print ( "Status: " );
    Serial.println (str_status[WiFi.status()]);
     //signal WiFi connect
    digitalWrite(BUILTIN_LED2, HIGH);
    delay(500); // ms
    digitalWrite(BUILTIN_LED2, LOW);
    delay(500); // ms 
    digitalWrite(BUILTIN_LED2, HIGH);     
  } else {
    Serial.println("");
    Serial.println("WiFi connect failed, push RESET button.");
    signalError();
  }

  telnetServer.begin();
  telnetServer.setNoDelay(true);
  Serial.println("Please connect Telnet Client, exit with ^] and 'quit'");

  //Serial.print("Free Heap[B]: ");
  //Serial.println(ESP.getFreeHeap());
} // setup()


void loop() {
  // look for Client connect trial
  if (telnetServer.hasClient()) {
    if (!serverClient || !serverClient.connected()) {
      if (serverClient) {
        serverClient.stop();
        Serial.println("Telnet Client Stop");
      }
      serverClient = telnetServer.available();
      //Serial.println("New Telnet client");
      serverClient.flush();  // clear input buffer, else you get strange characters 
    }
  }  
      commandString = ""; //clear the commandString variable
//
//      server.println("--> Please type your command and hit Return...");
//      alreadyConnected = true;
//    } 

    while(serverClient.available()) {      
     char newChar = serverClient.read();  // read the bytes incoming from the client:
     if (newChar == 0x0D){  //If a 0x0D is received, a Carriage Return, then evaluate the command
         serverClient.println();  
         serverClient.print("Received: ");
         serverClient.println(commandString);   
         Serial.println(commandString);
         processCommand(commandString);
         
         serverClient.flush();
         commandString = "";
         } 
      else{
       commandString += newChar;
     }

    }
  }


void processCommand(String command)
{
  if (commandString.indexOf("ron") > -1){
    //serverClient.println(command.indexOf("ron"));
    Serial.println("Relay ON");
    digitalWrite(RELAY1, HIGH);   // sets the LED on
    digitalWrite(BUILTIN_LED2, LOW);
    serverClient.println("Relay ON");
    return;
  } 
  
  if (command.indexOf("roff") > -1){
    Serial.println("Relay OFF");
    digitalWrite(RELAY1, LOW);   // sets the LED off
    digitalWrite(BUILTIN_LED2, HIGH);
    serverClient.println("Relay OFF");
    return;
  } 

    
  commandString = "";

}
