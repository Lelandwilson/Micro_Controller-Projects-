#include <Wire.h>
///////// do not use i2C_address 0, reservred for broadcast
const int SLAVE_ADDRESS = 2; //////////////Unit address Change for each unit

#define ADC_SCALE 1023.0
#define VREF 4.096
#define DEFAULT_FREQUENCY 50

#define IO_FrequencyPin 0
#define IO_VsensPin A0
#define IO_IsensPin A1

#define V_SCALEFACTOR 1065
#define I_CALIBRATION 6.5

#define WINDOW_SIZE 4

float ADCtoV_Conversion;

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
float Vmax_ADC = 0;
float Vmin_ADC = 99999;
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
  Serial.println("<INIT>");
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent); // register event

  analogReference(EXTERNAL); //////////!!!!!!!/////////

  //Pin 2 is Hardware interrupt 0
  pinMode(IO_FrequencyPin, INPUT_PULLUP);
  //Interrupt to pin 2
  attachInterrupt(IO_FrequencyPin, isrpulse, RISING);

  ADCtoV_Conversion = VREF / ADC_SCALE;

}

void loop() {
  //-- To occur contantly --//
  Vreading();
  Ireading();

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

    HVave = HVsum / VnoOfReadings;
    HVsum = 0;
    HVrms = (float)(sqrt(HVave));
    HVpeak = ((HVpeakFilter.getSmoothedValue() - ADC_V_biasedOffset) * ADCtoV_Conversion) * V_SCALEFACTOR;

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


    //
    //    Serial.println("VoltRMS: " + String(HVrms) +" : "+ String(newHVrms));
    //    Serial.println("Current RMS: " + String(HIrms) +" : "+ String(newHIrms));
    //    Serial.println("CCF: " + String(CCF) +" : "+ String(newCCF));
    //    Serial.println("ThetaS:" + String(theta_deg) +" : "+ String(newtheta_deg));
    //
    //    Serial.println("Apparent_Power: " + String(Apparent_Power) +" : "+ String(newApparent_Power));
    //    Serial.println("Real_Power: " + String(Real_Power) +" : "+ String(newReal_Power));
    //    Serial.println("Reactive_Power: " + String(Reactive_Power) +" : "+ String(newReactive_Power));
    //    Serial.println("PF: " + String(Power_Factor) +" : "+ String(newPower_Factor));
    //    Serial.println("");

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

    Vmax_ADC = 0;
    Vmin_ADC = 99999;

    Imax_ADC = 0;
    Imin_ADC = 99999;

    V_ZeroCross = 99999;
    I_ZeroCross = 99999;
    Serial.flush();
  }
}

void Vreading() {
  int VinReading = analogRead(IO_VsensPin);
  VnoOfReadings ++;

  if (Vmin_ADC > VinReading) {
    Vmin_ADC =  VinReading;
    HVtroughFilter.addDataPoint(Vmin_ADC);
  }
  if (Vmax_ADC < VinReading) {
    Vmax_ADC = VinReading;
    HVpeakFilter.addDataPoint(Vmax_ADC);
  }

  float hvmax = HVpeakFilter.getSmoothedValue();
  float hvmin = HVtroughFilter.getSmoothedValue();

  ADC_V_biasedOffset = (((hvmax - hvmin) / 2) + hvmin);

  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  LVsample = ((VinReading - ADC_V_biasedOffset) * ADCtoV_Conversion);
  HVconverted = LVsample * V_SCALEFACTOR;
  //Serial.println(HVconverted);

  if (HVconvertedPREV <= 0 && HVconverted >= 0 && V_ZeroCross == 99999) {
    V_ZeroCross = millis() - lastRead;
  }


  //LVsum += LVsample * LVsample;
  HVsum += HVconverted * HVconverted; //squared to convert negs to positives and accumlate readings

  HVconvertedPREV = HVconverted;
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


void requestEvent() {
  if (transmitFlag == true) {
    if (!firstPayloadSent) {
      Wire.write(payload1); // respond with message of 29 bytes
      firstPayloadSent = true;
    }
    else {
      Wire.write(payload2); // respond with message of 29 bytes
      firstPayloadSent = false;
    }
    
    //Serial.println("Sent");
  }

}

void isrpulse() {
  pulse++;
}
