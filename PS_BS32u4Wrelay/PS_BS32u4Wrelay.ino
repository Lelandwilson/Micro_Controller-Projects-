#include <SPI.h>
#include <RH_RF95.h>


//  for feather32u4
#define RFM95_CS 1
#define RFM95_RST 4
#define RFM95_INT 7


//for Wemos mini
//#define RFM95_CS D8 // *Chip select: "NSS" on Lora
//#define RFM95_RST D2 // *Restet: "Reset" on Lora
//#define RFM95_INT D1 // *"DI00" on Lora
//#include <ESP8266WiFi.h>
/*/

 for feather m0
  #define RFM95_CS 8
  #define RFM95_RST 4
  #define RFM95_INT 3
*/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt
#define LED 13
String  MSG = "";
String Webstring = "";
int WebStringLength = 27;  //56 for 1 reading
int noReadings = 2;   /// Number of readings start at 0

String ReceiverID = "RXX0";

String ProbeID[10] {"", "", "", "", "", "", "", "", "", ""};
String Lightlvl[10] {"", "", "", "", "", "", "", "", "", ""};
String Templvl[10] {"", "", "", "", "", "", "", "", "", ""};
String Humlvl[10] {"", "", "", "", "", "", "", "", "", ""};
String SoilMlvl[10] {"", "", "", "", "", "", "", "", "", ""};
String Batlvl[10] {"", "", "", "", "", "", "", "", "", ""};
String Recplvl[10] {"", "", "", "", "", "", "", "", "", ""};

const char* ssid = "SSID HERE";
const char* password = "PASS HERE";


void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  //while (!Serial);
  Serial.begin(115200);
  delay(100);

  Serial.println("LoRa Receiver");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    /////Serial.println("LoRa radio init failed");
    while (1);
  }
  /////Serial.println("LoRa radio initialized");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    /////Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);

  //-----WIFI setup-----//

  // Connect to WiFi network
  Serial.print("Connecting to: ");
  Serial.println(ssid);

  //WiFi.config(ip, gateway, subnet);

  //WiFi.config(ip, dns, gateway, subnet);
  //WiFi.config(ip, gateway, subnet, dns);
  delay(100);



}

void loop()
{

  sendDataRequest();
  Wait_receive();
  //netwrok();
  delay(50);

}



void sendDataRequest()
{
  //Serial.println("Sending data request");
  //MSG = "AAA1";
  if (Serial.available()) {      // If anything comes in Serial (USB),
    MSG += Serial.readString();   //
    if (MSG == "/n"); {
      //-Pass data string to rf95_server-//
      //Serial.println(MSG);
      char radiopacket[15] = "";
      MSG.toCharArray(radiopacket, 15);
      //Serial.println(radiopacket);

      //-Also print data string to serial-//
      //Serial.print(radiopacket);
      radiopacket[15] = 0;

      //-Send data via LoRa-//
      delay(10);
      rf95.send((uint8_t *)radiopacket, 15);
      delay(10);
      rf95.waitPacketSent();
      MSG = "";
    }

  }


}



void Wait_receive()
{
  // Should be a message for us now
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.recv(buf, &len))
  {
    if (Webstring.length() >= (len * noReadings) )
    {
      //Serial.println("###");
      //Serial.println(Webstring);
      //Serial.println("###");

      Webstring = "";
    }

    digitalWrite(LED, HIGH);
    Serial.print((char*)buf); //Received string
    Serial.print(rf95.lastRssi(), DEC);   //Signal strength
    Serial.println(">");
    delay(10);
    Webstring += (char*)buf + String(rf95.lastRssi(), DEC) + "> ";

    //Webstring = ((char*)buf);

    // Send a reply
    uint8_t data[] = "RXX0";    //ReceiverID
    rf95.send(data, sizeof(data));
    //rf95.send(ReceiverID, sizeof(ReceiverID));
    rf95.waitPacketSent();
    //Serial.println("Sent a reply");
    digitalWrite(LED, LOW);
  }
  else
  {
    //Serial.println("Receive failed");
  }
}
