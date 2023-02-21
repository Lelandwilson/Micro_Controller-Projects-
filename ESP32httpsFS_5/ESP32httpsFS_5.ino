
#include "defines.h"
#include "W5500.h"
#include "http.h"
#include "INAmeasure.h"
#include "WSserver.h"
#include "alerts.h"

/////////////////

void setup() {
    Serial.begin(115200);
    Serial.println("\n---<VERSION:" + Version + "| HARDWARE: "+ Hardware + ">---\n");

    pinMode(LED_BUILTIN, OUTPUT);
    
    pinMode(LV_R1, OUTPUT); 
    pinMode(LV_R2, OUTPUT);
    pinMode(LV_R3, OUTPUT);
    pinMode(LV_R4, OUTPUT);
    
    pinMode(HV_R1, OUTPUT);
    pinMode(HV_R2, OUTPUT);
    pinMode(_240Detectpin, INPUT);
    
    dht.setup(dhtPin, DHTesp::DHT11);
    
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    
    digitalWrite(LED_BUILTIN, LOW);

    digitalWrite(LV_R1, HIGH); 
    digitalWrite(LV_R2, LOW);
    digitalWrite(LV_R3, LOW);
    digitalWrite(LV_R4, LOW);

    digitalWrite(HV_R1, LOW);
    digitalWrite(HV_R2, LOW);

    Serial.println("  ------------");
    Serial.println("  |-LV_R1: " + String(digitalRead(LV_R1))); 
    Serial.println("  |-LV_R2: " + String(digitalRead(LV_R2))); 
    Serial.println("  |-LV_R3: " + String(digitalRead(LV_R3))); 
    Serial.println("  |-LV_R4: " + String(digitalRead(LV_R4))); 

    Serial.println("  |-HV_R2: " + String(digitalRead(HV_R1))); 
    Serial.println("  |-HV_R2: " + String(digitalRead(HV_R2))); 
    Serial.println("  ------------");

    ina219_A.begin();  // Initialize first board (default address 0x40)
    ina219_B.begin();  // Initialize second board with the address 0x41

    INA_0 = INAmeasure(0); //Volt and current for system
    INA_1 = INAmeasure(1); //Volt and current for battery
     
    // reserve 200 bytes for the WSinputString:
    WSinputString.reserve(200);
    
    //SPIFFS.format();  /////////////////REQUIRED FOR FIRST TIME FORMATTING OF NEW UNIT
    if (!SPIFFS.begin()) {
      Serial.println("An Error has occurred while mounting SPIFFS");
      Serial.println("Attempting to Format SPIFFS");
      SPIFFS.format();  /////////////////REQUIRED FOR FIRST TIME FORMATTING OF NEW UNIT
  
      delay(5000);
      return;
    }
    Serial.println("\n<Loading files from memory>");
    ///////// User settings load //
    File User_file = SPIFFS.open("/USERsettings.txt", FILE_READ);
    if (!User_file) {
      File User_file = SPIFFS.open("/USERsettings.txt", FILE_WRITE);
      Serial.println("/USERsettings.txt not present");
      User_file.println("USER=" + String(default_userX));
      User_file.println("PASS=" + String(default_passX));
      User_file.close();
      delay(10);
    }
    while (User_file.available()) {Load_USER_settings += char(User_file.read());}
    if (Load_USER_settings.length()<5){
      Serial.println("Load_USER_settings Error, file too short. Using Default");
      User_file.close();
      SPIFFS.remove("/USERsettings.txt");
      File User_file = SPIFFS.open("/USERsettings.txt", FILE_WRITE);
      User_file.println("USER=" + String(default_userX));
      User_file.println("PASS=" + String(default_passX));
      User_file.close();
      delay(10);
      User_file = SPIFFS.open("/USERsettings.txt", FILE_READ);
      while (User_file.available()) {Load_USER_settings += char(User_file.read());}
      }
      
  Load_Settings += Load_USER_settings;

  Serial.println("Loaded:");
  Serial.print(Load_Settings);
  Serial.println("----------");
  
  User_file.close();

  Serial.print("Loading index.html for webserver: ");
  if(SPIFFS.exists("/index.html") == 1){
    Serial.println("True");
  }
  else{
    Serial.println("False");
  }


  httpSetup();
  //HTTP server begin
  if(HTTP_SRV == true){
    server.begin();
    Serial.print(F("HTTP Server Online: "));

    Serial.print(Ethernet.localIP());
    Serial.println(":" + String(Webport));
  }
  
  //Websocket begin
  WsServerInit();
  

  Serial.println("OTA port: 65280");

  ArduinoOTA.begin(Ethernet.localIP(), OTAuser, OTApass, InternalStorage);
  digitalWrite(LED_BUILTIN, LOW);

//
//  server.on("/", HTTP_GET, []() {    
//      //server.send(200, "text/html", readFile(SPIFFS, "/index.html"));
//      server.send(200, F("text/html"), readFile(SPIFFS, "/index.html"));
//  });

  server.serveStatic("/", SPIFFS, "/index.html");
  server.serveStatic("/settings", SPIFFS, "/settings.html");
  
  server.serveStatic("/style.css", SPIFFS, "/style.css");
  server.serveStatic("/js1.js", SPIFFS,"/js1.js"); // Javascript
   
  server.serveStatic("/cam1.jpg", SPIFFS, "http://admin:Lombardy1@192.168.0.201:80/ISAPI/Streaming/channels/101/picture");
//  server.serveStatic("/cam2", SPIFFS, "http://192.168.0.201/ISAPI/Streaming/channels/201/picture/");

//    server.on("/cam1.jpg", HTTP_GET, []() { 
//      //http://192.168.0.201:80/ISAPI/Streaming/channels/101/picture
//        server.send(200, "text/html", "http://admin:Lombardy1@192.168.0.201/ISAPI/Streaming/channels/101/picture");
//    });
//  server.serveStatic("/bootstrap.min.css", SPIFFS,"/bootstrap.min.css"); 
//  server.serveStatic("/jquery.slim.min.js", SPIFFS,"/jquery.slim.min.js"); 
//  server.serveStatic("/popper.min.js", SPIFFS,"/popper.min.js"); 
//  server.serveStatic("/bootstrap.bundle.min.js", SPIFFS,"/bootstrap.bundle.min.js"); 
//  server.serveStatic("/bootstrap_toggle.min.css", SPIFFS,"/bootstrap_toggle.min.css"); 
//  server.serveStatic("/bootstrap_toggle.min.js", SPIFFS,"/bootstrap_toggle.min.js"); 
  
  // server.serveStatic("/assets/font/fonticons.ttf", SPIFFS,"/assets/font/fonticons.ttf"); // font
  // server.serveStatic("/assets/picture.png", SPIFFS,"/assets/picture.png"); // Picture


//  server.serveStatic("/page1", SPIFFS, "/page1.html");
//  server.serveStatic("/page2", SPIFFS, "/page2.html");
//  server.serveStatic("/page3.", SPIFFS, "/page3.html");
//  server.serveStatic("/page4", SPIFFS, "/page4.html");

}



void loop(void){
  time_now = millis();
  if(timer_set == 0){
    timer_set = time_now;
  }
  if(time_now > timer_set + period){
    nonBlockingTimer();
    timer_set = 0;
  }

  
  wss.listen(); //Websocket server
  
  if(HTTP_SRV == true){
    server.handleClient(); //http server
    
  }
  
  ArduinoOTA.poll();// for ethernet OTA

}

void nonBlockingTimer(void){

  if(dht_b == true){
    HTreading = dhtRead();
  }

  if(INA_0_b == true){
    INA_0 = INAmeasure(0); //Volt and current for system    
  }

  if(INA_1_b == true){
  INA_1 = INAmeasure(1); //Volt and current for battery
  }

  if(_240VIN_b == true){
    if(digitalRead(_240Detectpin) == LOW){
      if(supplyOfflineAlert == false){
        // SEND Alert that power supply is OFFLINE 
        sendAlert("240V Supply is now OFFLINE");
        supplyOfflineAlert = true; // set falg to true to only send alert once.
      }
         
    }
    if(supplyOfflineAlert == true){
      if(digitalRead(_240Detectpin) == HIGH){
        //power supply has come back online
        // SEND Alert that power supply is back ONLINE 
        sendAlert("240V Supply is back ONLINE");
        supplyOfflineAlert = false;
      }
    }
 }

}
