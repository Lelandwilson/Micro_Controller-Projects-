void setup() {

  Serial.begin(9600);
//*****measure for your self!************
//As units are perfect. Or get pointometer 
//and calibrate to knon voltage
Serial.println("------------------");
Serial.println(" <Device init>");
Serial.println("------------------");

} 

void loop() {

 float resistor1 = 470000;
 float resistor2 = 100000;
 float denominator = (float)resistor2 / (resistor1 + resistor2);

  float voltage;
  //Obtain RAW voltage data
  //voltage = analogRead(A0);
  voltage = 12;
  //Convert to actual voltage referr to voltage divder equation and
  //*****measure for your self!
  voltage = (voltage / 1024)* 3.3;
  //Convert to voltage before divider
  //Divide by divider = multiply
  //Divide by 1/5 = multiply by 5
  voltage = voltage / denominator;
  //Output to serial
  Serial.print("Volts: ");
  Serial.println(voltage);
  //Delay to make serial out readable
  delay(500);
} // void loop close
