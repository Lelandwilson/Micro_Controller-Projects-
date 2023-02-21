void WizReset() {
  Serial.print("Resetting Wiz W5500 Ethernet Board...  ");
  pinMode(RESET_P, OUTPUT);
  digitalWrite(RESET_P, HIGH);
  delay(250);
  digitalWrite(RESET_P, LOW);
  delay(50);
  digitalWrite(RESET_P, HIGH);
  delay(350);
  Serial.println("Done.");
}

void prt_hwval(uint8_t refval) {
  switch (refval) {
  case 0:
      Serial.println("No hardware detected.");
      break;
  case 1:
      Serial.println("WizNet W5100 detected.");
      break;
  case 2:
      Serial.println("WizNet W5200 detected.");
      break;
  case 3:
      Serial.println("WizNet W5500 detected.");
      break;
  default:
      Serial.println
          ("UNKNOWN - Update espnow_gw.ino to match Ethernet.h");
  }
}

void prt_ethval(uint8_t refval) {
  switch (refval) {
  case 0:
      Serial.println("Uknown status.");
      break;
  case 1:
      Serial.println("Link flagged as UP.");
      break;
  case 2:
      Serial.println("Link flagged as DOWN. Check cable connection.");
      Serial.println("Restarting in 10 seconds");
      for(int i=0; i<10; i++ ){
        Serial.print(".");
        delay(1000);
      }
      ESP.restart();

  default:
      Serial.println
          ("UNKNOWN - Update espnow_gw.ino to match Ethernet.h");
  }
}

void httpSetup(){
  // Use Ethernet.init(pin) to configure the CS pin.
  Ethernet.init(5);           // GPIO5 on the ESP32.
  WizReset();

  
  if (DHCP_flag == false){
    // start the STATIC Ethernet connection:
    Serial.println("Starting ETHERNET connection...");
    Ethernet.begin(eth_MAC, eth_IP, eth_DNS, eth_GW, eth_MASK);
  }
  else{
    // start the DHCP Ethernet connection:
    Serial.println("Initialize Ethernet with DHCP:");
    if (Ethernet.begin(eth_MAC) == 0) {
      Serial.println("Failed to configure Ethernet using DHCP");
      
      Serial.println("Restarting in 10 seconds");
      delay(10000);

      ESP.restart();
    } 
    else {
      Serial.print("---->DHCP assigned IP ");
      Serial.println(Ethernet.localIP());
    }
  }

  delay(200);
  
  Serial.print("Ethernet IP is: ");
  Serial.println(Ethernet.localIP());


  
  Serial.print("Checking LAN connection.");
  bool rdy_flag = false;
  for (uint8_t i = 0; i <= 20; i++) {
      if ((Ethernet.hardwareStatus() == EthernetNoHardware)
          || (Ethernet.linkStatus() == LinkOFF)) {
          Serial.print(".");
          rdy_flag = false;
          delay(80);
      } else {
          rdy_flag = true;
          break;
      }
  }
  if (rdy_flag == false) {
      Serial.println
          ("\n\r\tHardware fault, or cable problem... cannot continue.");
      Serial.print("Hardware Status: ");
      prt_hwval(Ethernet.hardwareStatus());
      Serial.print("   Cable Status: ");
      prt_ethval(Ethernet.linkStatus());
      while (true) {
          delay(10);       
      }
  } 
  else {
      digitalWrite(LED_BUILTIN, HIGH);
      
      Serial.println(" OK");
  }
}
