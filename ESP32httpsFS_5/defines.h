#include <FS.h> // Spiffs
#include "SPIFFS.h" // Spiffs
#include <SPI.h> // Ethernet
#include <Ethernet.h> // Ethernet
#include <EthernetWebServer.h> // Ethernet webserver
#include <ETHArduinoOTA.h> // Ethernbet OTA updates
#include "DHTesp.h" // DHT11/22
#include <Wire.h> // INA219
#include <Adafruit_INA219.h> // INA219

Adafruit_INA219 ina219_A;
Adafruit_INA219 ina219_B(0x41);


#include <WebSocketServer.h>
using namespace net;
constexpr uint16_t port = 8081;
WebSocketServer wss{port};

String Version = "1";
String Hardware = "ESP32";

#define OTAuser "arduino"
#define OTApass "password"

///   HARDWARE ASSIGNMENTS   /////
#define LED_BUILTIN 2
int dhtPin = 17;
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define RESET_P 26        // Tie the Wiz820io/W5500 reset pin to ESP32 GPIO26 pin.
int LV_R1 = 33; //MODEM
int LV_R2 = 32; //FAN
int LV_R3 = 0; //SIREN
int LV_R4 = 4;  //AUX

int HV_R1 = 13; //SPOT LIGHT
int HV_R2 = 12; //SPARE

int _240Detectpin = 16; // 240V supply detector circuit

///////////////////////////////

int period = 20000;  //Non blocking timer period / delay
unsigned long time_now = 0;
unsigned long timer_set =0;
String HTreading;

IPAddress eth_IP(192, 168, 0, 121);     // Static IP address
IPAddress eth_MASK(255, 255, 255, 0);   // Subnet mask
IPAddress eth_DNS(192, 168, 0, 1);      // DNS server
IPAddress eth_GW(192, 168, 0, 1);       // Gateway (router)
int Webport = 80;


boolean alreadyConnected = false; // whether or not the client was connected previously
String WSinputString = "";         // a String to hold incoming data
bool WSstringComplete = false;  // whether the string is complete


bool DHCP_flag = false;
 
unsigned long last_time = 0;
bool led_state = false;
uint8_t eth_MAC[] = { 0x02, 0xF0, 0x0D, 0xBE, 0xEF, 0x01 };

String serialOutput = "";
const char* default_userX = "admin";
const char* default_passX = "esp32";
String Load_USER_settings = "";
String Load_Settings = "";
String INA_0; // String to hold voltage and currnt measurments of system
String INA_1; // String to hold voltage and currnt measurments of battery
bool INA_0_b = true;  //bool to hold the status of INA0, if it is activated for use or not
bool INA_1_b = true; //bool to hold the status of INA1, if it is activated for use or not
bool PIR0status = false; // bool to hold PIR0 status
bool _240VIN_b = true; //bool to hold the status of _240VIN, if it is activated for use or not
bool HTTP_SRV = true; //bool to hold status of http webserver
bool dht_b = true; // bool to hold the status of dht, if it is activated for use or not
bool supplyOfflineAlert = false;
DHTesp dht;
