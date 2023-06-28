#include <Wire.h>

#define i2C_Address 1 //////////////Unit address Change for each unit

//const int SLAVE_ADDRESS = 2;
const int SLAVE_ADDRESSES[5] = {2,3,4,5,6};
//const int SLAVE_ADDRESSES[] = {2, 3, 4, 5, 6};
int ARRAY_LENGTH;


const unsigned long PERIOD = 1000;
unsigned long lastTransmission = 0;

#define CHUNK_SIZE 30
char buffer[CHUNK_SIZE + 1]; // +1 for the null terminator

void setup() {
  Wire.begin(); // join i2c bus as a master
  Serial.begin(115200); // start serial comms
  Serial.println("INIT");

  ARRAY_LENGTH = sizeof(SLAVE_ADDRESSES)/sizeof(int); //determine the length of the i2c address array
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastTransmission >= PERIOD) {
    lastTransmission = currentMillis;
    // Serial.println("<PING>");

    for (int index = 0; index < ARRAY_LENGTH; index++) {
      // send data to the slave
      Serial.print("<00" + String(SLAVE_ADDRESSES[index]) + "> ");
      int dataToSend = 400;
      Wire.beginTransmission(SLAVE_ADDRESSES[index]);
      Wire.write((uint8_t*)&dataToSend, sizeof(int)); // sends the value 400
      byte error = Wire.endTransmission();

      if (error != 0) {
        // There was an error. The error code is in the 'error' variable.
        //      Serial.print("I2C error: ");
        //      Serial.println(error);
      }
      else {
        // request data from the slave
        Wire.requestFrom(SLAVE_ADDRESSES[index], 29); // request up to 30 bytes
        while (Wire.available()) { // slave may send less than requested
          char c = Wire.read(); // receive a byte as character
          Serial.print(c); // print the character
        }
        //Serial.println("");

        // request data from the slave again
        Wire.requestFrom(SLAVE_ADDRESSES[index], 29); // request up to 30 bytes
        while (Wire.available()) { // slave may send less than requested
          char c = Wire.read(); // receive a byte as character
          Serial.print(c); // print the character
        }
        Serial.println("");
      }
    }
    Serial.println("");
    Serial.flush();
  }
}
