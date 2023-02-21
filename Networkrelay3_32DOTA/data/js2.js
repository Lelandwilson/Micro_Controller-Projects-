  var xmlCount = 0;
  var xmlHttp=createXmlHttpObject();

  function createXmlHttpObject(){
   if(window.XMLHttpRequest){
      xmlHttp=new XMLHttpRequest();
   }else{
      xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');
   }
   return xmlHttp;
  }

  function process(){
   if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
     xmlHttp.open('PUT','xml',true);
     xmlHttp.onreadystatechange=handleServerResponse; // no brackets?????
     xmlHttp.send(null);
  
    xmlCount = xmlCount+1;


   }
   setTimeout('process()',1000);
  }

  function handleServerResponse(){
   if(xmlHttp.readyState==4 && xmlHttp.status==200){
     xmlResponse=xmlHttp.responseXML;

     xmldoc = xmlResponse.getElementsByTagName('T_idResponse');
     message = xmldoc[0].firstChild.nodeValue;
     document.getElementById('T_id').innerHTML=message;

     xmldoc = xmlResponse.getElementsByTagName('sigStrengthResponse');
     message = xmldoc[0].firstChild.nodeValue;
     document.getElementById('sigStrength_id_1').innerHTML='Wifi Signal Strength:'+ message;
     document.getElementById('sigStrength_id_2').innerHTML=message;

     xmldoc = xmlResponse.getElementsByTagName('r1Response');
     message = xmldoc[0].firstChild.nodeValue;



  $(document).ready(function() {
  });
