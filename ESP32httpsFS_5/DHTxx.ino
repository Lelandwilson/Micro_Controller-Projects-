
String dhtRead(){
  String dht_reading; 
  
  TempAndHumidity newValues = dht.getTempAndHumidity();
  if (dht.getStatus() != 0) {
    return("-1-1");
  }
  dht_reading = String(newValues.temperature) + ":" + String(newValues.humidity);
  Serial.println(dht_reading);
  return(dht_reading);

}
