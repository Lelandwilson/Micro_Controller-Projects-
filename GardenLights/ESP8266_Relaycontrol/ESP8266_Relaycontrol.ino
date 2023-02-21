#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

const char* ssid="Bloop_Wifi";
const char* Wifi_Password="Lombardy1";

const char* www_username = "admin";
const char* www_password = "esp8266";

// NTP settings //
const int daylightSavings = 3600;
const long utcOffsetInSeconds = 36000 + daylightSavings;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "oceania.pool.ntp.org", utcOffsetInSeconds);
//      ---       //



String webSite,javaScript,XML;
String UnitIP = "";
int Web_Port = 8088;
String Wifi_Strength = "";

int Hour_printcycle = 0;
int Hour_printlimit = 5000000;

ESP8266WebServer server(Web_Port);
int RELAY1 = D1;  

void buildWebsite(){
  buildJavascript();
  webSite="<!DOCTYPE HTML>\n";
  webSite+="<meta charset=\"UTF-8\">";
  webSite+="<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  webSite+="<link rel=\"stylesheet\" href=\"https://www.w3schools.com/w3css/4/w3.css\">";
  webSite+="<link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/css?family=Raleway\">";
  webSite+="<script src=\"http://ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js\"></script>";
  
  webSite+="<style>";
    webSite+="body,h1 {font-family: \"Raleway\", sans-serif}";
    webSite+="body, html {height: 100%}";
    webSite+=".bgimg {";
        webSite+="background-image: url('https://starwarsblog.starwars.com/wp-content/uploads/2020/04/star-wars-backgrounds-04.jpg');";
        webSite+="min-height: 100%;";
        webSite+="background-position: center;";
        webSite+="background-size: cover;";
    webSite+="}";

     webSite+=".switch {";
      webSite+="position: relative;";
      webSite+="display: inline-block;";
      webSite+="width: 60px;";
      webSite+="height: 34px;";
    webSite+="}";
    
    webSite+=".switch input {display:none;}";
    
    webSite+=".slider {";
      webSite+="position: absolute;";
      webSite+="cursor: pointer;";
      webSite+="top: 0;";
      webSite+="left: 0;";
      webSite+="right: 0;";
      webSite+="bottom: 0;";
      webSite+="background-color: #ccc;";
      webSite+="-webkit-transition: .4s;";
      webSite+="transition: .4s;";
    webSite+="}";
    
    webSite+=".slider:before {";
      webSite+="position: absolute;";
      webSite+="content: \"\";";
     webSite+=" height: 26px;";
      webSite+="width: 26px;";
      webSite+="left: 4px;";
      webSite+="bottom: 4px;";
      webSite+="background-color: white;";
      webSite+="-webkit-transition: .4s;";
      webSite+="transition: .4s;";
    webSite+="}";
    
    webSite+="input:checked + .slider {";
      webSite+="background-color: #2196F3;";
    webSite+="}";
    
    webSite+="input:focus + .slider {";
      webSite+="box-shadow: 0 0 1px #2196F3;";
    webSite+="}";
    
    webSite+="input:checked + .slider:before {";
      webSite+="-webkit-transform: translateX(26px);";
      webSite+="-ms-transform: translateX(26px);";
      webSite+="transform: translateX(26px);";
    webSite+="}";
    
    /* Rounded sliders */
    webSite+=".slider.round {";
      webSite+="border-radius: 34px;";
    webSite+="}";
    
    webSite+=".slider.round:before {";
      webSite+="border-radius: 50%;";
    webSite+="}";

//    webSite+="meter {";
//      webSite+="width: 126px;";
//      webSite+="height: 20px;";
//    webSite+="}";


    webSite+="meter {";
      webSite+="width: 125px;";
      webSite+="height: 20px;";
      webSite+="-webkit-appearance: none; /* Reset appearance */";
      webSite+="border: 1px solid #ccc;";
      webSite+="border-radius: 3px;";
    webSite+="}";

    
    webSite+="meter::-webkit-meter-bar {";
      webSite+="background: none; /* Required to get rid of the default background property */";
      webSite+="background-color: white;";
      webSite+="box-shadow: 0 5px 5px -5px #333 inset;";
    webSite+="}";
    
    webSite+="</style>";
    
    webSite+=javaScript;
    
    webSite+="<BODY onload='process()'>\n";
    webSite+="<div class=\"bgimg w3-display-container w3-animate-opacity w3-text-white\">";
    //webSite+="<div class=\"w3-display-topleft w3-padding-large w3-xlarge\">";
    //webSite+="Tsafe";
    //webSite+="</div>";
    webSite+="<div class=\"w3-display-middle\">";
      webSite+="<h1 class=\"w3-jumbo w3-animate-top\">Control Center</h1>";
      webSite+="<hr class=\"w3-border-grey\" style=\"margin:auto;width:20%\">";
      webSite+="<table>";
      //webSite+="<form action=\"/action_page.php\">"; //Change save

      webSite+="<tr><td><label for=\"Wifi_Strength\">Wifi Signal Strength:"+ Wifi_Strength +"db </label>";               
      webSite+="<td><meter max=-30 min=-80 value=\""+ Wifi_Strength +"\"></meter></td>";
      
      webSite+="<tr><td><label for=\"Blank_Space\"></label>";               
      webSite+="<td></td>";
      webSite+="<tr><td><label for=\"Blank_Space\"></label>";               
      webSite+="<td></td>";

      
      webSite+="<tr><td><label for=\"txtPIR1_Active\">Festoon Lights:</label>";               
      //webSite+="<td><label class=\"switch\"><input type=\"checkbox\" id=\"togBtn\" value=\"false\">";
      webSite+="<td><label class=\"switch\"><input type=\"checkbox\" id=\"S_R1\">";
        webSite+="<span class=\"slider round\"></span>";
      webSite+="</label></td>";


//     webSite+="<tr><td><button type=\"button\" id=\"B1\" class=\"btn btn-success\">Success</button></td>";
//     

      webSite+="<tr><td><label for=\"Blank_Space\"></label>";               
      webSite+="<td></td>";
      
      webSite+="<tr><td><label for=\"Blank_Space\"></label>";               
      webSite+="<td></td>";
      //webSite+="</form>";
      webSite+="</table>";
      
    webSite+="</div>";
    webSite+="<div class=\"w3-display-bottomright w3-padding-large\">";
    webSite+="\"<a href=\"/settingsPage\">Settings</a>";
    webSite+=" </div>";
    webSite+="</div>";
    webSite+="</BODY>\n";
  webSite+="</HTML>\n";
}



void buildJavascript(){
  javaScript="<SCRIPT>\n";
  javaScript+="var xmlHttp=createXmlHttpObject();\n";

  javaScript+="function createXmlHttpObject(){\n";
  javaScript+=" if(window.XMLHttpRequest){\n";
  javaScript+="    xmlHttp=new XMLHttpRequest();\n";
  javaScript+=" }else{\n";
  javaScript+="    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
  javaScript+=" }\n";
  javaScript+=" return xmlHttp;\n";
  javaScript+="}\n";

  javaScript+="function process(){\n";
  javaScript+=" if(xmlHttp.readyState==0 || xmlHttp.readyState==4){\n";
  javaScript+="   xmlHttp.open('PUT','xml',true);\n";
  javaScript+="   xmlHttp.onreadystatechange=handleServerResponse;\n"; // no brackets?????
  javaScript+="   xmlHttp.send(null);\n";
  javaScript+=" }\n";
  javaScript+=" setTimeout('process()',1000);\n";
  javaScript+="}\n";
  
  javaScript+="function handleServerResponse(){\n";
//  javaScript+=" if(xmlHttp.readyState==4 && xmlHttp.status==200){\n";
//  javaScript+="   xmlResponse=xmlHttp.responseXML;\n";
//  javaScript+="   xmldoc = xmlResponse.getElementsByTagName('Vresponse');\n";
//  javaScript+="   message = xmldoc[0].firstChild.nodeValue;\n";
//  javaScript+="   document.getElementById('Voltage').innerHTML=message;\n";
  javaScript+=" }\n";
  
//  javaScript+="}\n";


javaScript+="var switchStatus = false;\n";
javaScript+="$(document).ready(function() {\n";

javaScript+="$( \"#S_R1\" ).on('change', function() {\n";
javaScript+="    if ($(this).is(':checked')) {\n";
javaScript+="        switchStatus = $(this).is(':checked');\n";
javaScript+="        console.log('#S_R1 ON');\n";
javaScript+="        var updatedData = $.get('/Ron');\n";
//javaScript+="        updatedData.done(function(results){\n";
//javaScript+="        var data= {\n";
//javaScript+="        Status: results_relays.data\n";
//javaScript+="    };\n";



javaScript+="    }\n";
javaScript+="    else {\n";
javaScript+="       switchStatus = $(this).is(':checked');\n";
javaScript+="        console.log('#S_R1 OFF');\n";
javaScript+="        var updatedData = $.get('/Roff');\n";
javaScript+="    }\n";
javaScript+="});\n";    
javaScript+="});\n";   




//javaScript+="$(document).ready(function() {;\n";  
//javaScript+="    $(\"#B1\").click(function(){;\n";  
//javaScript+="        alert(\"button\");;\n";  
//javaScript+="    });\n";  
//javaScript+="});\n";  







javaScript+="</SCRIPT>\n";
}

void buildXML(){
  XML="<?xml version='1.0'?>";
  XML+="<response>";
  XML+="<Status_R1>";
  XML+=XML_R1();
  XML+="</Status_R1>";

//  XML+="<r1on>";
//  R1on();
//  XML+="</r1on>";


  
//  XML+="<Vresponse>";
//  XML+=XML_Volt();
//  XML+="</Vresponse>";
//

  
  XML+="</response>";
}
String XML_R1(){
    String Stringout = "";
    if (RELAY1 == HIGH){Stringout = "checked";}
    else{Stringout = "unchecked";}
    //Stringout = String(RELAY1);
    return Stringout;
    
}



void handleWebsite(){
  buildWebsite();
  server.send(200,"text/html",webSite);
}

void handleRon(){
  Serial.println("Relay ON");
  digitalWrite(RELAY1, HIGH); 
}

void handleRoff(){
  Serial.println("Relay OFF");
  digitalWrite(RELAY1, LOW); 
  
}

void handleXML(){
  buildXML();
  server.send(200,"text/xml",XML); //send XML to webserver
}

void get_Time(){
  timeClient.update();
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  delay (1000);
}

void setup() {
  ///////-----Relay------///////
  pinMode(RELAY1, OUTPUT);      // declare LED + as output

  digitalWrite(RELAY1, LOW);  
  
  Serial.begin(115200);  
  WiFi.begin(ssid,Wifi_Password);
  while(WiFi.status()!=WL_CONNECTED)delay(500);
  WiFi.mode(WIFI_STA);
  
  Serial.println("\n\n<Device Init>");
  
  Serial.print("Connected to ");
  Serial.println(ssid);
  
  Serial.print("Unit IP address: ");
  UnitIP = WiFi.localIP().toString();
  Serial.println(UnitIP);
  
  Serial.print("Signal Strength ");
  Wifi_Strength = String(WiFi.RSSI());
  Serial.println(WiFi.RSSI());
  
  //server.on("/",handleWebsite);
  server.on("/", [](){
  if(!server.authenticate(www_username, www_password))
    return server.requestAuthentication();
    handleWebsite();
  });
  server.on("/xml",handleXML);
  server.on("/Ron",handleRon);
  server.on("/Roff",handleRoff);
  server.on("/settingsPage", []() { //Define the handling function for the CSS path
    //server.send(200, "text/html", settingsPage);

  });
  server.begin();  
  
  timeClient.begin();  //NTP instance
}



void loop() {
   server.handleClient();

  

}
