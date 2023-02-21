/*
GPIO23 <--> MOSI
GPIO19 <--> MISO
GPIO18 <--> SCLK
GPIO5 <--> SCS
GPIO26 <--> RESET

GPIO14 - DO NOT USE, this breaks the W5500
GPIO27 - DO NOT USE, this breaks the W5500
///////////////////////

  /Users/leland/Library/Arduino15/packages/esp32/hardware/esp32/1.0.6/cores/esp32/Server.h //
  Must replace void begin in Server.h as below
 
  //virtual void begin(uint16_t port=0) =0;
  virtual void begin() =0;


  /Users/leland/Library/Arduino15/packages/esp32/hardware/esp32/1.0.6
  Must include platform.local.txt from new OTA ethernet library for OTA to work


  // INA219 I2C addresses //
  Default = 0x40  
  A0 soldered = 0x41
  A1 soldered = 0x44
  A0 and A1 soldered = 0x45




*/
