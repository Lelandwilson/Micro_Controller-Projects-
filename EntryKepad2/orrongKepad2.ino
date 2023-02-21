

// These constants won't change.  They're used to give names
// to the pins used:
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 10;    // the debounce time; increase if the output flickers

int pinPress = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

// A0 = button matrix
// D1 = Led backlight
// D2 = Soft Tx- 485 connection
// D3 = Soft Rx- 485 connection
// D5 = Buzzer
// D6 = REX request to exit
// D7 = Logic level converter for Wirgand 5V: D0
// D8 = Logic level converter for Wiegand 5V: D1

const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int ledPin = D1;
const int buzzer = D5;


const int W_D0 = D7; // Wiegand data 0 line is assigned to Arduino pin D7
const int W_D1 = D8; //   "       "  1  "            "            "    D8

// serial line input variables
boolean got_line; // 'true' on newline ('\n') reception
char buf[5]; // the input buffer
int index_X = 0; // current position in buffer

void outwiegbit(unsigned int b); // output one Wiegand bit
void outwieg26(uint32_t u32); // output a Wiegand-26 code
void process_line(const char str[]); // process the input line received from the serial port



int buttonState = LOW;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
int reading_State = LOW;
String inputCode = "";

char button_X;


void setup() {
  pinMode(ledPin, OUTPUT);
  // initialize serial communications at 9600 bps:
  Serial.begin(115200);
  
  digitalWrite(D1, HIGH);
  delay(500);
  digitalWrite(D1, LOW);
  delay(500);
  digitalWrite(D1, HIGH);

  pinMode(W_D0, OUTPUT);  
  pinMode(W_D1, OUTPUT);
  digitalWrite(W_D0, 1); // set line to IDLE state
  digitalWrite(W_D1, 1); // "             "
  
  got_line = false;  
  
  
}

void loop() {
  char reading;
  pinPress = analogRead(analogInPin);
  // If the switch changed, due to noise or pressing:
  if (reading_State != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();   
  }

  //    Digit 1    //
  if (pinPress >= 532 && pinPress <= 535){
      //Serial.println("1");
      button_X = '1';
      reading_State = HIGH;
  }
  //    Digit 2    //  
  else if (pinPress >= 439 && pinPress <= 441){
      //Serial.println("2");
      button_X = '2';
      reading_State = HIGH;
  }
  //    Digit 3    //
  else if (pinPress >= 364 && pinPress <= 366){
      //Serial.println("3");
      button_X = '3';
      reading_State = HIGH;
  }
  //    Digit 4    //
  else if (pinPress >= 570 && pinPress <= 574){
      //Serial.println("4");
      button_X = '4';
      reading_State = HIGH;
  }
  //    Digit 5    //
  else if (pinPress >= 464 && pinPress <= 467){
      //Serial.println("5");
      button_X = '5';
      reading_State = HIGH;
  }
  //    Digit 6    //
  else if (pinPress >= 381 && pinPress <= 386){
      //Serial.println("6");
      button_X = '6';
      reading_State = HIGH;
  }
  //    Digit 7    //
  else if (pinPress >= 605 && pinPress <= 608){
      //Serial.println("7");
      button_X = '7';
      reading_State = HIGH;
  }
  //    Digit 8    //
  else if (pinPress >= 486 && pinPress <= 489){
      //Serial.println("8");
      button_X = '8';
      reading_State = HIGH;
  }
  //    Digit 9    //
  else if (pinPress >= 396 && pinPress <= 400){
      //Serial.println("9");
      button_X = '9';
      reading_State = HIGH;
  }
  //    Digit 10    //
  else if (pinPress >= 650 && pinPress <= 654){
      //Serial.println("*");
      button_X = '*';
      reading_State = HIGH;
  }
  //    Digit *    //
  else if (pinPress >= 515 && pinPress <= 519){
      //Serial.println("0");
      button_X = '0';
      reading_State = HIGH;
  }
  //    Digit #    //
  else if (pinPress >= 414 && pinPress <= 418){
      //Serial.println("#");
      button_X = '#';
      reading_State = HIGH;
  }

  else{
    reading_State = LOW;
  }


  if ((millis() - lastDebounceTime) > debounceDelay) {
    // if the button state has changed:
    if (reading_State != buttonState) {
      buttonState = reading_State;

  
  if (buttonState == HIGH) {
        //ledState = !ledState;
        Serial.print("OUTPUT: ");
        Serial.println(button_X);
    
        //inputCode += button_X;
        buf[index_X++] = button_X;
         
      }
  
      if ((button_X == '*') || (index_X == sizeof(buf)-1)){
                buf[index_X] = '\0';
                got_line = true;
                //inputCode = "";
      }
      
        if (got_line == true){
          process_line(buf);
          got_line = false;
          index_X = 0;
        }
  
  
    delay(1);
    }
  }
}



// outputs ONE Wiegand bit
void outwiegbit(unsigned int b){
  int sel = b == 0 ? W_D0 : W_D1;
  digitalWrite(sel, 0);
  delayMicroseconds(80);
  digitalWrite(sel, 1);
  delayMicroseconds(240);
}

// outputs a 26 bit Wiegand code
// u32 is actually the 24-bit numeric code
void outwieg26(uint32_t u32){
  uint32_t tmp = u32;
  unsigned int p_even = 0;
  unsigned int p_odd = 1;
  // compute parity on trailing group of bits 
  for (int n=0; n<12; ++n)
  {
    p_odd ^= (tmp & 1);
    tmp >>= 1;
  }
  // compute parity on heading group of bits
  for (int n=12; n<24; ++n)
  {
    p_even ^= (tmp & 1);
    tmp >>= 1;
  }
  // now output data bits framed by parity ones
  outwiegbit(p_even);
  for (int n=0; n<24; ++n)
  {
    outwiegbit((u32 >> (23-n)) & 1);
  }
  outwiegbit(p_odd);  
}

// output just 'meaningful' numbers
void process_line(const char str[]){
  char msg[64];
  long l = atol(str);
  if (l < 0 || l > 0xFFFFFF)
  {
    Serial.write("ERROR\n");
    return;
  }
  sprintf(msg, "OK: %ld (0x%06lX)\n", l, l);
  Serial.write(msg);
  outwieg26((unsigned long) l);
  delay(1000);
}
