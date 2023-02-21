#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219_A;
Adafruit_INA219 ina219_B(0x41);

// INA219 I2C addresses //
//Default = 0x40
//A0 soldered = 0x41
//A1 soldered = 0x44
//A0 and A1 soldered = 0x45

String INAmeasure(int inaX){

  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  String Ina_device;
  String output;

  if(inaX == 0){
  shuntvoltage = ina219_A.getShuntVoltage_mV();
  busvoltage = ina219_A.getBusVoltage_V();
  current_mA = ina219_A.getCurrent_mA();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  Ina_device = "[A] System";
  }

  if(inaX == 1){
  shuntvoltage = ina219_B.getShuntVoltage_mV();
  busvoltage = ina219_B.getBusVoltage_V();
  current_mA = ina219_B.getCurrent_mA();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  Ina_device = "[B] Battery";
  }
  
  output = Ina_device + ": Bus Voltage:" + String(busvoltage) + " V\n";
  output += Ina_device + ": Shunt Voltage:" + String(shuntvoltage) + " mV\n";
  output += Ina_device + ": Load Voltage:" + String(loadvoltage) + " V\n";
  output += Ina_device + ": Current:" + String(current_mA) + " mA\n";
  return(output);
  //Serial.println(output);

//  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
//  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
//  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
//  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
//  Serial.println("");


}

void setup(void) {
  ina219_A.begin();  // Initialize first board (default address 0x40)
  ina219_B.begin();  // Initialize second board with the address 0x41
}


void loop(void){
  Serial.println(INAmeasure(0));
  delay(10);
  Serial.println(INAmeasure(1));
  delay(10000);

}
