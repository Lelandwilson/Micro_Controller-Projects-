
const int VoltSens = A0;
const int CurrentSens = A1;

float V_Read = 0;
float A_Read = 0;

float T_Vol = 0;
float T_AMP= 0;

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
  float VolHr = 0;
  float AmpHr = 0;
  for(int i = 0; i < 3600; i++) {
    Measure();
    
    Serial.print("<V:");
    Serial.print(V_Read);
    Serial.print(",A:");
    Serial.print(A_Read);
    Serial.println(">");
    delay(900);
  }
  VolHr = V_Read / 3600;
  AmpHr = A_Read / 3600;
  Serial.print("<V/Hr:");
  Serial.print(V_Read);
  Serial.print(",A/Hr:");
  Serial.print(A_Read);
  Serial.println(">");
  
} 

void Measure(){
  
  float resistor1 = 1000000;
  float resistor2 = 1100000;
  float denominator = (float)resistor2 / (resistor1 + resistor2);
  
  float voltage;
  //Obtain RAW voltage data
  voltage = analogRead(VoltSens);
  //Convert to actual voltage referr to voltage divder equation and
  //*****measure for your self!
  voltage = (voltage / 1024)* 3.3;
  V_Read = voltage / denominator;
  //Serial.print("Volts: ");
  //Serial.println(V_Read);
  
  float mVperAmp = 0.066;
  float voltageC = 0;
  //float vpp = 0.00488758553275; //For 5V
  //float vpp = 0.01173020527859; //For 12V
  float vpp = 0;
  float CurrentValue= 0;
  CurrentValue = 0;
  float average = 0;

  vpp = V_Read / 1024;
  
  for(int i = 0; i < 1001; i++) {
    CurrentValue += (analogRead(CurrentSens) -792) ;
  }

  average = (CurrentValue/1000);
  
  //A_Read = (average * mVperAmp); 

  A_Read = (average / mVperAmp);
  A_Read = A_Read * (vpp/2);
  A_Read = A_Read/2;  

  
  //Serial.print("    Amps: ");
  //Serial.println(A_Read);
}

