#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//Wemos//
static const int RXPin = D2, TXPin = D3;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);
int Counter1 = 0;
int DelayX = 1000000;

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);
  delay(100);
  Serial.println("--------------");
  Serial.println("<Device Init>");
  Serial.println("--------------");
}

void loop()
{
  Counter1 = Counter1 +1;
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      if (Counter1 >= DelayX){
        displayInfo();
        Counter1 = 0;
      }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("<No GPS detected: check wiring>"));
    while(true);
  }
}

void displayInfo()
{
  Serial.print(F("Loc: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("-"));
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }


  Serial.print(F("  D/T: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    //Serial.print(F("."));
    //if (gps.time.centisecond() < 10) Serial.print(F("0"));
    //Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
  //delay(5000);
}
