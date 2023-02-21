void WsRespondCommand(String instring, WebSocket &ws, bool serialPrint){
  if(serialPrint == true){
    Serial.println(instring);
  }
  instring = instring + "\n";
  int str_len = instring.length() + 1; 
  char char_array[str_len];
  instring.toCharArray(char_array, str_len);
  ws.send(WebSocket::DataType::TEXT, char_array, str_len);
}



void parseCommand(String input, WebSocket &ws){

  //LVR1_1 ...4       //Set LV relay 1-4 
  //HVR1_1 ...2       //Set HV relay 1-2 
  //REQ_TH            //Request temperature and humidity
  //REQ_INA0          //Request System Voltage and current readings
  //REQ_INA1          //Request Battery Voltage abd current readings
  //REQ_PIR0          //Request PIR sensor status
  //REQ_240VIN        //Request 240V Supply status
  //REQHTTP_SRV0 ..1 //Turn on/ off HTTP server


  ///// REQHTTP_SRV0 Request to query the HTTP webserver ///////
  if(input.indexOf("REQHTTP_SRV?") != -1){
    String sendTemp = "HTTP_" + String(HTTP_SRV);
    WsRespondCommand(sendTemp, ws, false);
  }
  
  ///// REQHTTP_SRV0 Request to shutdown the HTTP webserver ///////
  if(input.indexOf("REQHTTP_SRV0") != -1){
    server.close(); // close the HTTP server
    HTTP_SRV = false; // set the flag to false
    String sendTemp = "HTTP_" + String(HTTP_SRV);
    WsRespondCommand(sendTemp, ws, true);
  }


  ///// REQHTTP_SRV1 Request to start the HTTP webserver ///////
  if(input.indexOf("REQHTTP_SRV1") != -1){
    server.begin(); // close the HTTP server
    HTTP_SRV = true; // set the flag to false
    String sendTemp = "HTTP_" + String(HTTP_SRV);
    WsRespondCommand(sendTemp, ws, true);
  }

  ///// REQ_240VIN Request 240V Supply status ///////
  if(input.indexOf("REQ_240VIN") != -1){
    String sendTemp = "240VIN" + String(digitalRead(_240Detectpin));
    WsRespondCommand(sendTemp, ws, false);
  }


  ///// REQ_PIR0 Request PIR0 status ///////
  if(input.indexOf("REQ_PIR0") != -1){
    String sendTemp = "PIR0" + String(PIR0status);
    WsRespondCommand(sendTemp, ws, false);
  }
  
  ///// REQ_TH Request temperature and humidity ///////
  if(input.indexOf("REQ_TH") != -1){
    String sendTemp = "TEMP" + HTreading;
    WsRespondCommand(sendTemp, ws, false);
  }

  ////// REQ_INA0 Request System voltage and current ///////
  if(input.indexOf("REQ_INA0") != -1){
    INA_0 = INAmeasure(0); //Volt and current for system
    Serial.println(INA_0);
    
    String sendTemp = "INA0" + INA_0;
    WsRespondCommand(sendTemp, ws, false);
  }

 ////// REQ_INA1 Request Battery Voltage and Current ///////
  if(input.indexOf("REQ_INA1") != -1){
    INA_1 = INAmeasure(1); //Volt and current for system
    Serial.println(INA_1);
    
    String sendTemp = "INA1" + INA_1;
    WsRespondCommand(sendTemp, ws, false);
  }

  /////// LVR1 /////
  if(input.indexOf("LVR1_?") != -1){
    String output = "LVR1" + String(digitalRead(LV_R1));
    WsRespondCommand(output, ws, false);
  }
  if(input.indexOf("LVR1_1") != -1){
    digitalWrite(LV_R1, HIGH);
    String output = "LVR1" + String(digitalRead(LV_R1));
    Serial.println("Set- " + output);

    WsRespondCommand(output, ws, true);
  }
  if(input.indexOf("LVR1_0") != -1){
    digitalWrite(LV_R1, LOW);
    String output = "LVR1" + String(digitalRead(LV_R1));
    Serial.println("Set- " + output);

    WsRespondCommand(output, ws, true);
  }
  /////// LVR2 /////
  if(input.indexOf("LVR2_?") != -1){
    String output = "LVR2" + String(digitalRead(LV_R2));
    WsRespondCommand(output, ws, false);
  }
  if(input.indexOf("LVR2_1") != -1){
    digitalWrite(LV_R2, HIGH);
    String output = "LVR2" + String(digitalRead(LV_R2));
    Serial.println("Set- " + output);

    WsRespondCommand(output, ws, true);
  }
  if(input.indexOf("LVR2_0") != -1){
    digitalWrite(LV_R2, LOW);
    String output = "LVR2" + String(digitalRead(LV_R2));
    Serial.println("Set- " + output);

    WsRespondCommand(output, ws, true);
  }
  /////// LVR3 /////
  if(input.indexOf("LVR3_?") != -1){
    String output = "LVR3" + String(digitalRead(LV_R3));
    WsRespondCommand(output, ws, false);
  }
  if(input.indexOf("LVR3_1") != -1){
    digitalWrite(LV_R3, HIGH);
    String output = "LVR3" + String(digitalRead(LV_R3));
    Serial.println("Set- " + output);

    WsRespondCommand(output, ws, true);
  }
  if(input.indexOf("LVR3_0") != -1){
    digitalWrite(LV_R3, LOW);
    String output = "LVR3" + String(digitalRead(LV_R3));
    Serial.println("Set- " + output);

    WsRespondCommand(output, ws, true);
  }
  /////// LVR4 /////
  if(input.indexOf("LVR4_?") != -1){
    String output = "LVR4" + String(digitalRead(LV_R4));
    WsRespondCommand(output, ws, false);
  }
  if(input.indexOf("LVR4_1") != -1){
    digitalWrite(LV_R4, HIGH);
    String output = "LVR4" + String(digitalRead(LV_R4));
    Serial.println("Set- " + output);

    WsRespondCommand(output, ws, true);
  }
  if(input.indexOf("LVR4_0") != -1){
    digitalWrite(LV_R4, LOW);
    String output = "LVR4" + String(digitalRead(LV_R4));

    WsRespondCommand(output, ws, true);
  }
    /////// HVR1 /////
  if(input.indexOf("HVR1_?") != -1){
    String output = "HVR1" + String(digitalRead(HV_R1));
    WsRespondCommand(output, ws, false);
  }
  if(input.indexOf("HVR1_1") != -1){
    digitalWrite(HV_R1, HIGH);
    String output = "HVR1" + String(digitalRead(HV_R1));
    Serial.println("Set- " + output);

    WsRespondCommand(output, ws, true);
  }
  if(input.indexOf("HVR1_0") != -1){
    digitalWrite(HV_R1, LOW);
    String output = "HVR1" + String(digitalRead(HV_R1));
    Serial.println("Set- " + output);

    WsRespondCommand(output, ws, true);
  }
    /////// HVR2 /////
  if(input.indexOf("HVR2_?") != -1){
    String output = "HVR2" + String(digitalRead(HV_R2));
    WsRespondCommand(output, ws, false);
  }
  if(input.indexOf("HVR2_1") != -1){
    digitalWrite(HV_R2, HIGH);
    String output = "HVR2" + String(digitalRead(HV_R2));
    Serial.println("Set- " + output);

    WsRespondCommand(output, ws, true);
  }
  if(input.indexOf("HVR2_0") != -1){
    digitalWrite(HV_R2, LOW);
    String output = "HVR2" + String(digitalRead(HV_R2));
    Serial.println("Set- " + output);

    WsRespondCommand(output, ws, true);
  }
  
}


void WsServerInit(){
  Serial.print(F("WSockectServer running at "));
  Serial.print(Ethernet.localIP());
  Serial.print(F(":"));
  Serial.println(port);

  wss.onConnection([](WebSocket &ws) {
    const auto protocol = ws.getProtocol();
    if (protocol) {
      Serial.print(F("Client protocol: "));
      Serial.println(protocol);
    }

    ws.onMessage([](WebSocket &ws, const WebSocket::DataType dataType,
                   const char *message, uint16_t length) {
      switch (dataType) {
      case WebSocket::DataType::TEXT:
        Serial.print(F("Received: "));
        Serial.println(message);
        parseCommand(message, ws); //////////parse wsocket message
        
        break;
      case WebSocket::DataType::BINARY:
        Serial.println(F("Received binary data"));
        break;
      }

      //ws.send(dataType, message, length);
    });

    ws.onClose([](WebSocket &, const WebSocket::CloseCode, const char *,
                 uint16_t) { Serial.println(F("Disconnected")); });

    Serial.print(F("New WS client: "));
    Serial.println(ws.getRemoteIP());
    const char message[]{"<Connected>"};
    ws.send(WebSocket::DataType::TEXT, message, strlen(message));

  });

  wss.begin();

  
}
