EthernetWebServer server(Webport);

String readFile(fs::FS &fs, const char * path){
    String output = "";
    //Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return ("-1");
    }

    //Serial.println("- read from file:");
    while(file.available()){
        output += char(file.read());
    }
    file.close();
    return(output);
}



void handleNotFound(){
  String message = F("File Not Found\n\n");
  
  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? F("GET") : F("POST");
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");
  
  for (uint8_t i = 0; i < server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  
  server.send(404, F("text/plain"), message);
  
}
