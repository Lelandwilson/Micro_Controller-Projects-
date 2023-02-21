#include <Arduino.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif
//#include <OneWire.h> //for waterproof temp sensor
//##include <DallasTemperature.h> //for waterproof temp sensor
#include <dht.h>
dht DHT;
 
#include <SPI.h>
#include <RH_RF95.h>

#define DHT11_PIN 3    // Original is pin 1
//#define DHTPOWER 13
#define MainDelay 950
#define VBATPIN A3     //Original is A7
 
/* for feather32u4 */
#define RFM95_CS 8  // or 8    or 1
#define RFM95_RST 4
#define RFM95_INT 7
 

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0

String UnitID = "RRR1";
String  MSG = "";
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

uint8_t R1 = 2;
uint8_t R2 = 3;
uint8_t R3 = 5;
uint8_t R4 = 6;
String inputString = ""; 
 
void setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(R3, OUTPUT);
  pinMode(R4, OUTPUT);
  digitalWrite(R1, HIGH);
  digitalWrite(R2, HIGH);
  digitalWrite(R3, HIGH);
  digitalWrite(R4, HIGH);
  inputString.reserve(200);

 
  //while (!Serial);
  Serial.begin(115200);
  delay(100);
 
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}
 
int16_t packetnum = 0;  // packet counter, we increment per xmission


void loop()
{
   if (rf95.available())
  {
    //-Scan for message from base station reqsting data-//   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    
    
    if (rf95.recv(buf, &len))
    {
      inputString = (char*)buf;
      //Serial.print((char*)buf);
      if (UnitID == inputString.substring(0,4))
      //if (UnitID == ((char*)buf)) //Check if its a message for us
        {
         Serial.print("Recieved Message: ");
         Serial.println(inputString);
         
         //readings();
         relayX();
         sendMsg();
         inputString = "";
        }
      else
      {
        inputString = "";
       //Serial.println("Pass"); 
      }
      
     }
    else
    {
      //Serial.println("Receive failed");
    }
  }

}


void readings()
{
  MSG = "";
  //-DHT Temp & Humidity sensor  -  Send the command to get temperatures
  int chk = DHT.read11(DHT11_PIN);
  
  //-Collect sensor data-//  
  MSG += (UnitID);
  MSG += ("<");
  
  MSG += (int(DHT.temperature));
  MSG += (",");
  
  MSG += (int(DHT.humidity));
  MSG += (",");
  
  int Lux = analogRead(A4);
  float voltage1 = Lux * (100.0 / 1023.0);
  
  MSG += (int(voltage1));
  MSG += (",");
  
  int SoilMoistsensor = analogRead(A5);
  float voltage0 = SoilMoistsensor;
  voltage0 = map(voltage0, 250, 850, 0, 100);
  voltage0 = map(voltage0, 0, 100, 100, 0);
  
  MSG += (int(voltage0));
  MSG += (",");


  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  measuredvbat = map(measuredvbat, 0, 4.9, 0, 100);
  
  MSG += (int(measuredvbat));
  MSG += (",");
  //-End of sensor data collection-//
}  

void sendMsg()
{
  //String  MSG = "";
  //-Pass data string to rf95_server-//
  char radiopacket[24] = "";
  MSG.toCharArray(radiopacket,24);

  //-Also print data string to serial-//
  Serial.print("Response: ");
  Serial.println(radiopacket);
  ////Serial.println("X>");
  radiopacket[23] = 0;

  //-Send data via LoRa-// 
  delay(10);
  rf95.send((uint8_t *)radiopacket, 24);
  delay(10);
  rf95.waitPacketSent();
  MSG = "";

}

 
void relayX()
{
  //inputString = (char*)buf); 
  if (inputString.indexOf("1_ON")!= -1){
          Serial.println("Action: RELAY 1: ON");
          digitalWrite(R1, LOW);
          MSG += ("RELAY 1: ON");
          }
        
        else if (inputString.indexOf("2_ON")!= -1){
          Serial.println("Action: RELAY 2: ON");
          digitalWrite(R2, LOW);
          MSG += ("RELAY 2: ON");
          }
          
        else if (inputString.indexOf("3_ON")!= -1){
          Serial.println("Action: RELAY 3: ON");
          digitalWrite(R3, LOW);
          MSG += ("RELAY 3: ON");
          }

        else if (inputString.indexOf("4_ON")!= -1){
          Serial.println("Action: RELAY 4: ON");
          digitalWrite(R4, LOW);
          MSG += ("RELAY 4: ON");
          }
          
        else if (inputString.indexOf("1_OFF")!= -1){
          Serial.println("Action: RELAY 1: OFF");
          digitalWrite(R1, HIGH);
          MSG += ("RELAY 1: OFF");
          }
        else if (inputString.indexOf("2_OFF")!= -1){
          Serial.println("Action: RELAY 2: OFF");
          digitalWrite(R2, HIGH);
          MSG += ("RELAY 2: OFF");
          }
        else if (inputString.indexOf("3_OFF")!= -1){
          Serial.println("Action: RELAY 3: OFF");
          digitalWrite(R3, HIGH);
          MSG += ("RELAY 3: OFF");
          }
        else if (inputString.indexOf("4_OFF")!= -1){
          Serial.println("Action: RELAY 4: OFF");
          digitalWrite(R4, HIGH);
          MSG += ("RELAY 4: OFF");
          }

        else if (inputString.indexOf("OK")!= -1){
          Serial.println("Action: OK");
          MSG += ("OK");
          }
  
        else{
          Serial.println(inputString);
          Serial.println("Action: ERROR");
          MSG = ("ERROR_invalid");
          }
        
}



