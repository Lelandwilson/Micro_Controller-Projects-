#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/////////////////--//////////////////
const char* ssid="SSID HERE";
const char* Wifi_Password="WIFI PASSWORD HERE";

const char* www_username = "admin";
const char* www_password = "esp8266";

String webSite,javaScript,XML;
String UnitIP = "";
int Web_Port = 8088;
ESP8266WebServer server(Web_Port);
/////////////////-\/VPIRs\/-//////////////////
const int Relay1 = D1;
String R1stat = "OFF";


String EmailNotify = "Email notifications= OFF";
String AppNotify = "App notifications= OFF";




void buildWebsite(){
  buildJavascript();
  webSite="<!DOCTYPE HTML>\n";
  webSite+="<meta charset=\"UTF-8\">";
  webSite+="<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  webSite+="<link rel=\"stylesheet\" href=\"https://www.w3schools.com/w3css/4/w3.css\">";
  webSite+="<link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/css?family=Raleway\">";
  
  webSite+="<style>";
    webSite+="body,h1 {font-family: \"Raleway\", sans-serif}";
    webSite+="body, html {height: 100%}";
    webSite+=".bgimg {";
        webSite+="background-image: url('https://starwarsblog.starwars.com/wp-content/uploads/2020/04/star-wars-backgrounds-04.jpg');";
        webSite+="min-height: 100%;";
        webSite+="background-position: center;";
        webSite+="background-size: cover;";
    webSite+="}";



    /* The switch - the box around the slider */
    webSite+=".switch {";
      webSite+="position: relative;";
      webSite+="display: inline-block;";
      webSite+="width: 60px;";
      webSite+="height: 34px;";
    webSite+="}";
    
    /* Hide default HTML checkbox */
    webSite+=".switch input {";
      webSite+="opacity: 0;";
      webSite+="width: 0;";
      webSite+="height: 0;";
    webSite+="}";
    
    /* The slider */
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
      webSite+="content: "";";
      webSite+="height: 26px;";
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






    
    webSite+="</style>";
    
    webSite+=javaScript;
    
    webSite+="<BODY onload='process()'>\n";
    webSite+="<div class=\"bgimg w3-display-container w3-animate-opacity w3-text-white\">";
    webSite+="<div class=\"w3-display-topleft w3-padding-large w3-xlarge\">";
      webSite+="<h1>Garden Control Center</h1>";
    webSite+="</div>";
    webSite+="<div class=\"w3-display-middle\">";
      webSite+="<h3 class=\"w3-jumbo w3-animate-top\">Garden Lights:</h3>";
      webSite+="<hr class=\"w3-border-grey\" style=\"margin:auto;width:40%\">";


      webSite+="<!-- Rounded switch -->";
//      webSite+="<p class=\"w3-large w3-center\">Festoon Lights<A id='Festoon'></A></p>";
       webSite+="<p class=\"w3-large w3-center\">";
         webSite+="<label class=\"switch\">";
           webSite+="<input type=\"checkbox\" id=\"togBtn\" value=\"false\" name=\"disableYXLogo\">";
          webSite+="<span class=\"slider round\"></span>";
        webSite+="</label>";


// webSite+="<label class=\"switch\">";
// webSite+="<input type=\"checkbox\" id=\"togBtn\" value=\"false\" name=\"disableYXLogo\">";
// webSite+="<div class=\"slider round\"></div>";
// webSite+="</label>";


        
      webSite+="</p>";

    webSite+="</div>";
    
    webSite+="<div class=\"w3-display-bottomright w3-padding-large\">";
    webSite+="\"<a href=\"/settingsPage\">Settings</a>";
    webSite+=" </div>";
    webSite+="</div>";

    webSite+="</BODY>\n";
  webSite+="</HTML>\n";
}

void buildPage2(){
  buildJavascript();
  webSite="<!DOCTYPE HTML>\n";
//  webSite+="<meta charset=\"UTF-8\">";
//  webSite+="<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
//  webSite+="<link rel=\"stylesheet\" href=\"https://www.w3schools.com/w3css/4/w3.css\">";
//  webSite+="<link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/css?family=Raleway\">";
//  
//  webSite+="<style>";
//    webSite+="body,h1 {font-family: \"Raleway\", sans-serif}";
//    webSite+="body, html {height: 100%}";
//    webSite+=".bgimg {";
//        webSite+="background-image: url('https://starwarsblog.starwars.com/wp-content/uploads/2020/04/star-wars-backgrounds-04.jpg');";
//        webSite+="min-height: 100%;";
//        webSite+="background-position: center;";
//        webSite+="background-size: cover;";
//    webSite+="}";
//    webSite+="</style>";
//    
//    webSite+=javaScript;
//    
//    webSite+="<BODY onload='process()'>\n";
//    webSite+="<div class=\"bgimg w3-display-container w3-animate-opacity w3-text-white\">";
//    webSite+="<div class=\"w3-display-topleft w3-padding-large w3-xlarge\">";
//    webSite+="Control Center";
//    webSite+="</div>";
//    webSite+="<div class=\"w3-display-middle\">";
//      webSite+="<h1 class=\"w3-jumbo w3-animate-top\">Settings</h1>";
//      webSite+="<hr class=\"w3-border-grey\" style=\"margin:auto;width:40%\">";
//      webSite+="<p class=\"w3-large w3-center\">Ip Address:      <input type=\"text\" name=\"Ip_address\" value=\""+UnitIP +"\"size=\"15\">";
//      webSite+="<p class=\"w3-large w3-center\">Port:      <input type=\"text\" name=\"Web_port\" value=\""+String(Web_Port) +"\"size=\"15\">";
//      webSite+="<p class=\"w3-large w3-center\">Wifi SSID:      <input type=\"text\" name=\"Wifi_SSID\" value=\""+ String(ssid) +"\"size=\"15\">";
//      webSite+="<p class=\"w3-large w3-center\">Wifi Password:      <input type=\"text\" name=\"Wifi_Password\" value=\""+ String(Wifi_Password) +"\"size=\"15\">";
//      webSite+="<p class=\"w3-large w3-center\">Login Username:      <input type=\"text\" name=\"User_Name\" value=\""+ String(www_username) +"\"size=\"15\">";
//      webSite+="<p class=\"w3-large w3-center\">login Password:      <input type=\"text\" name=\"User_Password\" value=\""+ String(www_password) +"\"size=\"15\">";
//      
//      webSite+="<p class=\"w3-large w3-center\">Ip Address: "+ UnitIP +"<A id='Ip_address'></A></p>";
//      webSite+="<p class=\"w3-large w3-center\">Port: "+ String(Web_Port) +"<A id='Web_port'></A></p>";
//      webSite+="<p class=\"w3-large w3-center\">Wifi SSID: "+ String(ssid) +"<A id='Wifi_SSID'></A></p>";
//      webSite+="<p class=\"w3-large w3-center\">Wifi Password: "+ String(Wifi_Password) +"<A id='Wifi_Password'></A></p>";
//      webSite+="<p class=\"w3-large w3-center\">Login Username: "+ String(www_username) +"<A id='User_Name'></A></p>";
//      webSite+="<p class=\"w3-large w3-center\">login Password: "+ String(www_password) +"<A id='User_Password'></A></p>";
////      webSite+="<p class=\"w3-large w3-center\">PIR1 Active: <A id='PIR1_Active'></A></p>";
////      webSite+="<p class=\"w3-large w3-center\">PIR2 Active: <A id='PIR2_Active'></A></p>";
////      webSite+="<p class=\"w3-large w3-center\">PIR Sensitivity: <A id='PIR_Sensitivity'></A></p>";
////      webSite+="<p class=\"w3-large w3-center\">Accelerometer Active: <A id='Accelerometer_Active'></A></p>";
////      webSite+="<p class=\"w3-large w3-center\">Accelerometer Sensitivity: <A id='Accelerometer_Sensitivity'></A></p>";
////      webSite+="<p class=\"w3-large w3-center\">Wifi Signal Strength: <A id='Wifi_Strength'></A></p>";
////      webSite+="<p class=\"w3-large w3-center\">Reset factory Default<A id='Reset_Default'></A></p>";
//    webSite+="</div>";
//    webSite+="<div class=\"w3-display-bottomright w3-padding-large\">";
//    webSite+="\"<a href=\"/\">Home</a>";
//    webSite+=" </div>";
//    webSite+="</div>";
//    webSite+="</BODY>\n";
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
  javaScript+=" if(xmlHttp.readyState==4 && xmlHttp.status==200){\n";
  javaScript+="   xmlResponse=xmlHttp.responseXML;\n";
  javaScript+="   xmldoc = xmlResponse.getElementsByTagName('R1response');\n";
  javaScript+="   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript+="   document.getElementById('Voltage').innerHTML=message;\n";

  
  javaScript+=" }\n";
  javaScript+="}\n";
  javaScript+="</SCRIPT>\n";
}

void buildXML(){
  XML="<?xml version='1.0'?>";
  XML+="<response>";
//  XML+="<Vresponse>";
//  XML+=XML_Volt();
//  XML+="</Vresponse>";

  XML+="<R1response>";
  XML+=XML_R1();
  XML+="<R1response>";



  
  XML+="</response>";
}
String XML_R1(){
    R1Status();
    String Stringout = "";
    Stringout = R1stat;
    return Stringout;
    
}


void R1Status(){
 if(D1 == HIGH){
   R1stat = "ON";
 }
 else{
  R1stat = "OFF";
  }
}



//String XML_Volt(){
//    MV1();
//    String Stringout = "";
//    Stringout = voltageString;
//    return Stringout;
//    
//}


void handleWebsite(){
  buildWebsite();

  server.send(200,"text/html",webSite);
}

void handlePage2(){
  buildPage2();
  server.send(200,"text/html",webSite);
}

void handleXML(){
  buildXML();
  server.send(200,"text/xml",XML);
}

void setup() {
  ///////-----PIR------///////
  pinMode(Relay1, OUTPUT);      // declare LED + as output

  Serial.begin(115200);  
  WiFi.begin(ssid,Wifi_Password);
  while(WiFi.status()!=WL_CONNECTED)delay(500);
  WiFi.mode(WIFI_STA);
  Serial.println("\n\nBOOTING ESP8266 ...");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("Station IP address: ");
  UnitIP = WiFi.localIP().toString();
  Serial.println(UnitIP);
  //server.on("/",handleWebsite);
  server.on("/", [](){
  if(!server.authenticate(www_username, www_password))
    return server.requestAuthentication();
    handleWebsite();
  });
  server.on("/xml",handleXML);
  server.on("/settingsPage", []() { //Define the handling function for the CSS path
    //server.send(200, "text/html", settingsPage);
    handlePage2();
  });
  server.begin();  
}



void loop() {
   server.handleClient();

}
