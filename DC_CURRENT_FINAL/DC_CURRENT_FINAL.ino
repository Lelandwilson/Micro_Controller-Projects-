const int CurrentSens = A1;
float mVperAmp = 0.066;
float voltage = 0;
//float vpp = 0.00488758553275;
float vpp = 0.01260997067449;
float CurrentValue= 0;
float adjusted = 0;

void setup() {
Serial.begin(9600);
}

void loop() {
  CurrentValue = 0;
  float average = 0;
  for(int i = 0; i < 10000; i++) {
    CurrentValue += (analogRead(CurrentSens) -789.5) ;
  }
  average = CurrentValue/10000;
  voltage = average * vpp;
  //if(average < 0.0){average = 0.0;}
  //Serial.println(average);
  adjusted = voltage / mVperAmp;
  Serial.println(adjusted);
  
  

}
