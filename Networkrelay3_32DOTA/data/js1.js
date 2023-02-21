
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
     xmlHttp.open('PUT','/xml',true);
     xmlHttp.onreadystatechange=handleServerResponse; 
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


     if (xmlCount == 1) {
         if (message== 'checked'){

            $('#toggle-one').bootstrapToggle('on')
         }
     }
     else {
         if (xmlCount == 1) {
             $('#toggle-one').bootstrapToggle('off')
         }
     }

     if (xmlCount!= xmlCount) {
         if (message== 'checked'){

            $('#toggle-one').bootstrapToggle('on')
         }
     }
     else {
         if (xmlCount!= xmlCount) {
             $('#toggle-one').bootstrapToggle('off')
         }
     }

   }
  }


  $(document).ready(function() {
    $('#toggle-one').on('change', function() {
        if ($(this).is(':checked')) {
              switchStatus = $(this).is(':checked');
              var updatedData = $.post('/Ron');
        }

        else {
             switchStatus = $(this).is(':checked');
             var updatedData = $.post('/Roff');
        }
    });
  });
