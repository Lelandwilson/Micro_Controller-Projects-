#include <Wire.h>
#include <WireSlave.h>
///////// do not use i2C_address 0, reservred for broadcast
const int SLAVE_ADDRESS = 0x02; //////////////Unit address Change for each unit

#define SDA_PIN 21
#define SCL_PIN 22

#define ADC_SCALE 4095.0
#define VREF 3.3
#define DEFAULT_FREQUENCY 50

#define IO_FrequencyPin 32

#define IO_VsensPin 34
#define IO_IsensPin 35

#define V_SCALEFACTOR 1065
#define I_CALIBRATION 1 //6.5

#define WINDOW_SIZE 4

float ADCtoV_Conversion;

void analogSetAttenuation(adc_attenuation_t attenuation); // set default adc attenuation to 11db
void analogSetWidth(uint8_t bits); // set deafulat adc resolution to 12 bit: 4045
/*
* Set pin to use for ADC calibration if the esp is not already calibrated (25, 26 or 27)
* */
void analogSetVRefPin(uint8_t pin);

/*
* Set the divider for the ADC clock.
* Default is 1
* Range is 1 - 255
* */
void analogSetClockDiv(uint8_t clockDiv);

/////// I2C VARS /////////
int i2cCall = 0;
#define PAYLOAD_SIZE 30
char payload1[PAYLOAD_SIZE];
char payload2[PAYLOAD_SIZE];
volatile bool transmitFlag = false;
bool firstPayloadSent = false;


/////// FREQUENCY VARS //////////
volatile unsigned long pulse = 0;
unsigned long lastRead = 0;
unsigned long interval = 1000;//1 second
int frequency = 0;

/////// VOLTAGE VARS //////////
int VsensPin = IO_VsensPin;
float LVsample = 0;
float HVconverted = 0;
float HVconvertedPREV = 0;
int VnoOfReadings = 0;
float HVave = 0;
float HVsum = 0;
float HVrms = 0;
float HVpeak = 0;
float Vmax_ = -99999;
float Vmin_ = 99999;
float ADC_V_biasedOffset = 0.0;

float V_ZeroCross = 99999;

/////// CURRENT VARS //////////
int IsensPin = IO_IsensPin;
float LIsample = 0;
float HIconverted = 0;
float HIconvertedPREV = 0;
int InoOfReadings = 0;
float HIave = 0;
float HIsum = 0;
float HIrms = 0;
float HIpeak = 0;
//float biasedOffset = 0;
float Imax_ADC = 0;
float Imin_ADC = 99999;
float ADC_I_biasedOffset = 0.0;
float I_ZeroCross = 99999;

float i_CreastFactor = 0.0;

float I_mVperAmp = (20.0 + I_CALIBRATION) * 1000;

//////// POWER VARS /////////
float Apparent_Power = 0.0;
float Active_Power = 0.0;
float Reactive_Power = 0.0;
float Power_Factor = 0.0;


void IRAM_ATTR isr() {
  pulse++;
}

class MovingAverageFilter {
  public:
    MovingAverageFilter(int size) : size(size), index(0), sum(0) {
      buffer = new float[size]();
    }

    ~MovingAverageFilter() {
      delete[] buffer;
    }

    void addDataPoint(float dataPoint) {
      sum -= buffer[index];
      buffer[index] = dataPoint;
      sum += buffer[index];
      index = (index + 1) % size;
    }

    float getSmoothedValue() {
      return sum / size;
    }

    void printArray() {
      Serial.print("[");
      for (int i = 0; i <= sizeof(buffer) + 1; i++) {
        Serial.print(buffer[i]);
        Serial.print(", ");
      }
      Serial.println("]");
    }

  private:
    int size;
    int index;
    float sum;
    float* buffer;
};


MovingAverageFilter HVpeakFilter(WINDOW_SIZE);
MovingAverageFilter HIpeakFilter(WINDOW_SIZE);
MovingAverageFilter HVtroughFilter(WINDOW_SIZE);
MovingAverageFilter HItroughFilter(WINDOW_SIZE);

void setup() {
  Serial.begin(115200);
  setCpuFrequencyMhz(240);
  Serial.println("<INIT>");
  
  Serial.print("CPU speed: ");
  Serial.println(getCpuFrequencyMhz()); //Get CPU clock
  
//  Wire.begin(SLAVE_ADDRESS);
//  Wire.onRequest(requestEvent); // register event

 bool success = WireSlave.begin(SDA_PIN, SCL_PIN, SLAVE_ADDRESS);
    if (!success) {
        Serial.println("I2C slave init failed");
        while(1) delay(100);
    }

//    WireSlave.onReceive(receiveEvent);
    WireSlave.onReceive(requestEvent);


  //Pin 2 is Hardware interrupt 0
  pinMode(IO_FrequencyPin, INPUT_PULLUP);
  //Interrupt to pin 2
  attachInterrupt(IO_FrequencyPin, isr, RISING);



  ADCtoV_Conversion = VREF / ADC_SCALE;

}

void loop() {
  //-- To occur contantly --//
  Vreading();
  Ireading();
//  WireSlave.update();

  //-- To occur every second --//
  if (millis() - lastRead >= interval) {
    lastRead  += interval;
    //Serial.println("<Tick>");
    // disable interrupts, make copy of pulse, reenable interrupts
    noInterrupts();
    frequency = pulse / 2;
    pulse = 0;

    HIave = HIsum / InoOfReadings;
    HIsum = 0;
    HIrms = (float)(sqrt(HIave));
    HIpeak = ((HIpeakFilter.getSmoothedValue() - ADC_I_biasedOffset) * ADCtoV_Conversion) * I_mVperAmp;
//    HIpeak = ((Imax_ADC - ADC_I_biasedOffset) * ADCtoV_Conversion) * I_mVperAmp;


    HVave = HVsum / VnoOfReadings;
    HVsum = 0;
    HVrms = (float)(sqrt(HVave));
    HVpeak = ((HVpeakFilter.getSmoothedValue() - ADC_V_biasedOffset) * ADCtoV_Conversion) * V_SCALEFACTOR;
//    HVpeak = ((Vmax_ADC - ADC_V_biasedOffset) * ADCtoV_Conversion) 
    
    interrupts();

    float CCF = HIpeak / HIrms;
    float period = 1.0 / frequency;
    float theta_deg = ((I_ZeroCross - V_ZeroCross) / (period * 1000)) * 360; // θ
    float theta_rad = theta_deg * PI / 180.0;

    float Apparent_Power = HVrms * (HIrms / 1000); // S
    float Real_Power = Apparent_Power * cos(theta_rad); // P also "Active Power"
    float Reactive_Power = Apparent_Power * sin(theta_rad); // Q
    float Power_Factor = cos(theta_rad); // PF

    // Convert from float to int fro TXing
    int newHVrms = floor(HVrms * 100);
    int newHIrms = floor(HIrms);
    int newCCF = floor(CCF * 100);
    int newtheta_deg = floor(theta_deg * 100);

    int newApparent_Power = floor(Apparent_Power);
    int newReal_Power = floor(Real_Power);
    int newReactive_Power = floor(Reactive_Power);
    int newPower_Factor = floor(Power_Factor * 100);




    String readings = "";

    if (frequency != 0) {
      snprintf(payload1, PAYLOAD_SIZE, "[%02d,%03d,%03d,%05d,%03d,%04d",
               frequency, VnoOfReadings, newHVrms, newHIrms, newCCF, newtheta_deg);
      //Serial.println(payload1);

      snprintf(payload2, PAYLOAD_SIZE, ",%07d,%07d,%07d,%03d]",
               newApparent_Power, newReal_Power, newReactive_Power, newPower_Factor);
      //Serial.println(payload2);
      transmitFlag = true;

      readings += "Frequency:   " + String(frequency) + " Hz\n";
      readings += "Samples:     " + String(VnoOfReadings) + "\n";
      readings += "RMS V:       " + String(HVrms) + " V\n";
      readings += "RMS I:       " + String(HIrms) + " mA\n";
      readings += "CCF:         " + String(CCF) + "\n";
      readings += "Theta:       " + String(theta_deg) + " deg\n";
      readings += "Apparent P:  " + String(Apparent_Power) + " VA\n";
      readings += "Real P:      " + String(Real_Power) + "W\n";
      readings += "Reactive P:  " + String(Reactive_Power) + " VAR\n";
      readings += "PF:          " + String(Power_Factor) + " \n";
      Serial.println(readings);
    }
    else {
      snprintf(payload1, PAYLOAD_SIZE, "%s", "[No AC Detected           ]");
      snprintf(payload2, PAYLOAD_SIZE, "%s", "                    ");
      Serial.println("No AC Detected");
    }


    //Reset values for recalibration
    InoOfReadings = 0;
    VnoOfReadings = 0;

    Vmax_ = -99999;
    Vmin_ = 99999;

    Imax_ADC = -99999;
    Imin_ADC = 99999;

    V_ZeroCross = 99999;
    I_ZeroCross = 99999;
    Serial.flush();
  }
}

void Vreading() {
  int adcReading = analogRead(IO_VsensPin);
  VnoOfReadings ++;
  float voltage = (adcReading / 4095.0) * 3.3; //now an LV measurement
  float estimatedHvVoltage = (voltage - 1.213) /0.001000; //V_SCALEFACTOR;  //Now a HV measurement


  if (Vmin_ > estimatedHvVoltage) {
    Vmin_ =  estimatedHvVoltage;
    HVtroughFilter.addDataPoint(Vmin_);
  }
  if (Vmax_ < estimatedHvVoltage) {
    Vmax_ = estimatedHvVoltage;
    HVpeakFilter.addDataPoint(Vmax_);
  }

  if (HVconvertedPREV <= 0 && estimatedHvVoltage >= 0 && V_ZeroCross == 99999) {
    V_ZeroCross = millis() - lastRead;
  }

  HVsum += estimatedHvVoltage * estimatedHvVoltage; //squared to convert negs to positives and accumlate readings

  HVconvertedPREV = estimatedHvVoltage;
}

void Ireading() {
  int Isample = analogRead(IO_IsensPin);
  InoOfReadings ++;

  if (Imin_ADC > Isample) {
    Imin_ADC =  Isample;
    HItroughFilter.addDataPoint(Imin_ADC);
  }
  if (Imax_ADC < Isample) {
    Imax_ADC = Isample;
    HIpeakFilter.addDataPoint(Imax_ADC);
  }

  float himax = HIpeakFilter.getSmoothedValue();
  float himin = HItroughFilter.getSmoothedValue();

  ADC_I_biasedOffset = (((himax - himin) / 2) + himin);


  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  LIsample = ((Isample - ADC_I_biasedOffset) * ADCtoV_Conversion);
  HIconverted = I_mVperAmp * LIsample;
  //Serial.println(HIconverted);

  if (HIconvertedPREV <= 0 && HIconverted >= 0 && I_ZeroCross == 99999) {
    I_ZeroCross = millis() - lastRead;
  }


  //LIsum += LIsample * LIsample;
  HIsum += HIconverted * HIconverted; //squared to convert negs to positives and accumlate readings

  HIconvertedPREV = HIconverted;
}


void requestEvent(int howMany) {
  if (transmitFlag == true) {
    if (!firstPayloadSent) {
      WireSlave.write(payload1); // respond with message of 29 bytes
      firstPayloadSent = true;
    }
    else {
      WireSlave.write(payload2); // respond with message of 29 bytes
      firstPayloadSent = false;
    }
    
    //Serial.println("Sent");
  }

}

// function that executes whenever a complete and valid packet
// is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
    while (1 < WireSlave.available()) // loop through all but the last byte
    {
        char c = WireSlave.read();  // receive byte as a character
        Serial.print(c);            // print the character
    }

    int x = WireSlave.read();   // receive byte as an integer
    Serial.println(x);          // print the integer
}
