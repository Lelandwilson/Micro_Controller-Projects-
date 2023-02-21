
/*jshint esversion: 6 */
setInterval(function() {
   get_Status();
}, 20000);

function smlog(inString){
    if(inString.substring(0,6)!= "[SENT]"){
        terminalData += "[RECEIVED] " + inString;
    }
    else{
        terminalData += inString;
    }
    
    $("#smText").text(terminalData);
    var $textarea = $('#smText');
    $textarea.scrollTop($textarea[0].scrollHeight);
}

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

 var terminalData;

//var gateway = `ws://${window.location.hostname}:8081`;
var gateway = `ws://${window.location.hostname}:8081`;
var websocket;
window.addEventListener('load', onLoad);
function initWebSocket() {
    //console.log('Trying to open a WebSocket connection...');
    smlog('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
}
function onOpen(event) {
//console.log('Connection opened');
smlog('Connection opened\n');
get_Status();
get_relayStatus();
}
function onClose(event) {
// console.log('Connection closed');
smlog('Connection closed\n');
    setTimeout(initWebSocket, 2000);
}
function onMessage(event) {
    //////// Read command in form device /////////
    // console.log("Received: " + event.data);



    ////// INA0[A] ///////
    if (event.data.substring(0,7) == "INA0[A]"){
        var text = event.data;
        let Aval = text.indexOf("Bus Voltage:",0) + 12;
        let Bval = text.indexOf("Shunt Voltage:",0) + 14;
        let Cval = text.indexOf("Load Voltage:",0) + 13;
        let Dval = text.indexOf("Current:",0) + 8;

        let firstline = text.indexOf("\n",0); // Bus Voltage:
        let secondline = text.indexOf("\n",firstline + 1); // Shunt Voltage:
        let thirdline = text.indexOf("\n",secondline + 1); // Load Voltage:
        let fourthline = text.indexOf("\n",thirdline + 1); // Current:

        var AbusV = event.data.substring(Aval,firstline);
        var AshuntV = event.data.substring(Bval,secondline); 
        var AloadV = event.data.substring(Cval,thirdline); 
        var AcurrentI = event.data.substring(Dval,fourthline); 

        // document.getElementById("supplyStatus").className = "badge badge-warning";
        document.getElementById("systemStatusBusV").innerHTML = AbusV;
        document.getElementById("systemStatusV").innerHTML = AloadV;
        document.getElementById("systemStatusI").innerHTML = AcurrentI;

    }
    ////// INA1[B] ///////
    if (event.data.substring(0,7) == "INA1[B]"){
        var text2 = event.data;
        let Aval = text2.indexOf("Bus Voltage:",0) + 12;
        let Bval = text2.indexOf("Shunt Voltage:",0) + 14;
        let Cval = text2.indexOf("Load Voltage:",0) + 13;
        let Dval = text2.indexOf("Current:",0) + 8;

        let firstline = text2.indexOf("\n",0); // Bus Voltage:
        let secondline = text2.indexOf("\n",firstline + 1); // Shunt Voltage:
        let thirdline = text2.indexOf("\n",secondline + 1); // Load Voltage:
        let fourthline = text2.indexOf("\n",thirdline + 1); // Current:

        var BbusV = event.data.substring(Aval,firstline);
        var BshuntV = event.data.substring(Bval,secondline); 
        var BloadV = event.data.substring(Cval,thirdline); 
        var BcurrentI = event.data.substring(Dval,fourthline); 

        // document.getElementById("supplyStatus").className = "badge badge-warning";
        document.getElementById("batteryStatusBusV").innerHTML = BbusV;
        document.getElementById("batteryStatusV").innerHTML = BloadV;
        document.getElementById("batteryStatusI").innerHTML = BcurrentI;

    }



    // 240V Supply //
    if (event.data.substring(0,6) == "240VIN"){
        if(event.data.substring(6,7) == "0"){
            document.getElementById("supplyStatus").innerHTML = "OFFLINE!";
            document.getElementById("supplyStatus").className = "badge badge-warning";
            smlog("Warning: 240V Supply is OFFLINE\n");
            // console.log("Warning: 240V Supply is OFFLINE");

        }
        if(event.data.substring(6,7) == "1"){
            document.getElementById("supplyStatus").innerHTML = "Online";
            document.getElementById("supplyStatus").className = "badge badge-secondary";
            // smlog("240V Supply is ONLINEn");
            // console.log("Warning: 240V Supply is ONLINE");

        }
    }

    // PIR0 //
    if (event.data.substring(0,4) == "PIR0"){
        // let PIR = document.getElementById("pirStatus");
        if(event.data.substring(4,5) == "0"){           
            // PIR.className = "badge badge-sucess";
            document.getElementById("pirStatus").className = "badge badge-secondary";
            document.getElementById("pirStatus").innerHTML = "Clear";
            // smlog("PIR Sensor: No Motion Detected"));
            // console.log("PIR Sensor: No Motion Detected");

        }
        if(event.data.substring(4,5) == "1"){
            // PIR.className = "badge badge-warning";
            document.getElementById("pirStatus").className = "badge badge-warning";
            document.getElementById("pirStatus").innerHTML = "Motion Detected";
            smlog("PIR Sensor: Motion Detected\n");
            // console.log("PIR Sensor: Motion Detected");

        }
    }

    // TEMPERATURE //
    if (event.data.substring(0,4) == "TEMP"){

        let tempString = ".   Temp: " + event.data.substring(4,9) + "°C";
        let humString = ". Humidity: " + event.data.substring(10) + "%";
        document.getElementById("temp_").innerHTML = tempString + humString;

    }
    // LVR1 // event.data.substring(0,6) == "TEMP"
    if (event.data.substring(0,5) == "LVR10"){
      if($('#tb_LV_R1').is(':checked') == true){
            $('#tb_LV_R1').bootstrapToggle('off');
            smlog("LVR1 off\n");
            // console.log("LVR1 off");
        }
        else{
            smlog(event.data.substring(0,5) + " OFF ACK\n");
        }
    }
    if (event.data.substring(0,5) == "LVR11"){
            if($('#tb_LV_R1').is(':checked') == false){
            $('#tb_LV_R1').bootstrapToggle('on');
            smlog("LVR1 on\n");
            // console.log("LVR1 on");
        }
        else{
            smlog(event.data.substring(0,5) + " ON ACK\n");
        }
    }
    // LVR2 //
    if (event.data.substring(0,5) == "LVR20"){
      if($('#tb_LV_R2').is(':checked') == true){
            $('#tb_LV_R2').bootstrapToggle('off');
            smlog("LVR2 off\n");
            // console.log("LVR2 off");
        }
        else{
            smlog(event.data.substring(0,5) + " OFF ACK\n");
        }
    }
    if (event.data.substring(0,5) == "LVR21"){
            if($('#tb_LV_R2').is(':checked') == false){
            $('#tb_LV_R2').bootstrapToggle('on');
            smlog("LVR2 on\n");
            // console.log("LVR2 on");
        }
        else{
            smlog(event.data.substring(0,5) + " ON ACK\n");
        }
    }
    // LVR3 //
    if (event.data.substring(0,5) == "LVR30"){
      if($('#tb_LV_R3').is(':checked') == true){
            $('#tb_LV_R3').bootstrapToggle('off');
            smlog("LVR3 off\n");
            // console.log("LVR3 off");
        }
        else{
            smlog(event.data.substring(0,5) + " OFF ACK\n");
        }
    }
    if (event.data.substring(0,5) == "LVR31"){
            if($('#tb_LV_R3').is(':checked') == false){
            $('#tb_LV_R3').bootstrapToggle('on');
            smlog("LVR3 on\n");
            // console.log("LVR3 on");
        }
        else{
            smlog(event.data.substring(0,5) + " ON ACK\n");
        }
    }
    // LVR4 //
    if (event.data.substring(0,5) == "LVR40"){
      if($('#tb_LV_R4').is(':checked') == true){
            $('#tb_LV_R4').bootstrapToggle('off');
            smlog("Receive: LVR4 off\n");
            // console.log("Receive: LVR4 off");
        }
        else{
            smlog(event.data.substring(0,5) + " OFF ACK\n");
        }
    }
    if (event.data.substring(0,5) == "LVR41"){
            if($('#tb_LV_R4').is(':checked') == false){
            $('#tb_LV_R4').bootstrapToggle('on');
            smlog("LVR4 on\n");
            // console.log("LVR4 on");
        }
        else{
            smlog(event.data.substring(0,5) + " ON ACK\n");
        }
    }
    // HVR1 //
    if (event.data.substring(0,5) == "HVR10"){
      if($('#tb_HV_R1').is(':checked') == true){
            $('#tb_HV_R1').bootstrapToggle('off');
            smlog("HVR1 off\n");
            // console.log("HVR1 off");
        }
        else{
            smlog(event.data.substring(0,5) + " OFF ACK\n");
        }
    }
    if (event.data.substring(0,5) == "HVR11"){
            if($('#tb_HV_R1').is(':checked') == false){
            $('#tb_HV_R1').bootstrapToggle('on');
            smlog("HVR1 on\n");
            // console.log("HVR1 on");
        }
        else{
            smlog(event.data.substring(0,5) + " ON ACK\n");
        }
    }
    // HVR2 //
    if (event.data.substring(0,5) == "HVR20"){
      if($('#tb_HV_R2').is(':checked') == true){
            $('#tb_HV_R2').bootstrapToggle('off');
            smlog("HVR2 off\n");
            // console.log("HVR2 off");
        }
        else{
            smlog(event.data.substring(0,5) + " OFF ACK\n");
        }
    }
    if (event.data.substring(0,5) == "HVR21"){
            if($('#tb_HV_R2').is(':checked') == false){
            $('#tb_HV_R2').bootstrapToggle('on');
            smlog("HVR2 on\n");
            // console.log("HVR2 on");
        }
        else{
            smlog(event.data.substring(0,5) + " ON ACK\n");
        }
    }
}
function onLoad(event) {
    initWebSocket();
}

function get_Status(){
    websocket.send('REQ_TH'); //Request temperature and humidity readings
    sleep(1000).then(() => { websocket.send('REQ_INA0'); }); //Request System Voltage and Current readings
    sleep(1000).then(() => { websocket.send('REQ_INA1'); }); //Request Batetry Voltage and Current readings
    sleep(1000).then(() => { websocket.send('REQ_PIR0'); }); //Request PIR status
    sleep(1000).then(() => { websocket.send('REQ_240VIN'); }); //Request 240V supply status

}

function get_relayStatus(){
    websocket.send('LVR1_?');
    websocket.send('LVR2_?');
    websocket.send('LVR3_?');
    websocket.send('LVR4_?');
    websocket.send('HVR1_?');
    websocket.send('HVR2_?');

}


$(document).ready(function() {
    //websocket.send('REQ_TH');

    $('#smContainer').hide();

    var smon = document.getElementById("serialMonitorButton");
    smon.onclick = function() {
        $('#smContainer').toggle();
        return false;
    };
    var dt = new Date();
    document.getElementById("date_").innerHTML = dt.toLocaleDateString();

    //////// Send command (from web page to device) to trigger relays ///////////
    // LVR1 //
    $('#tb_LV_R1').on('change', function() {
        if ($(this).is(':checked')) {
            switchStatus = $(this).is(':checked');
            websocket.send('LVR1_1');
            smlog("[SENT] LVR1 ON\n");
            // console.log("Send: LVR1 ON");
        }

        else {
            switchStatus = $(this).is(':checked');
            websocket.send('LVR1_0');
            smlog("[SENT] LVR1 OFF\n");
            // console.log("Send: LVR1 OFF");
        }
    });
    // LVR2 //
    $('#tb_LV_R2').on('change', function() {
        if ($(this).is(':checked')) {
            switchStatus = $(this).is(':checked');
            websocket.send('LVR2_1');
            smlog("[SENT] LVR2 ON\n");
            // console.log("Send: LVR2 ON");
        }

        else {
             switchStatus = $(this).is(':checked');
             websocket.send('LVR2_0');
             smlog("[SENT] LVR2 OFF\n");
             // console.log("Send: LVR2 OFF");
        }
    });
    // LVR3 //
    $('#tb_LV_R3').on('change', function() {
        if ($(this).is(':checked')) {
              switchStatus = $(this).is(':checked');
              websocket.send('LVR3_1');
              smlog("[SENT]LVR3 ON\n");
              // console.log("Send: LVR3 ON");
        }

        else {
             switchStatus = $(this).is(':checked');
             websocket.send('LVR3_0');
             smlog("[SENT] LVR3 OFF\n");
             // console.log("Send: LVR3 OFF");
        }
    });
    // LVR4 //
    $('#tb_LV_R4').on('change', function() {
        if ($(this).is(':checked')) {
              switchStatus = $(this).is(':checked');
              websocket.send('LVR4_1');
              smlog("[SENT] LVR4 ON\n");
              // console.log("Send: LVR4 ON");
        }

        else {
             switchStatus = $(this).is(':checked');
             websocket.send('LVR4_0');
             smlog("[SENT] LVR4 OFF\n");
             // console.log("Send: LVR4 OFF");
        }
    });
    // HVR1 //
    $('#tb_HV_R1').on('change', function() {
        if ($(this).is(':checked')) {
              switchStatus = $(this).is(':checked');
              websocket.send('HVR1_1');
              smlog("[SENT] HVR1 ON\n");
              // console.log("Send: HVR1 ON");
        }

        else {
             switchStatus = $(this).is(':checked');
             websocket.send('HVR1_0');
             smlog("[SENT] HVR1 OFF\n");
             // console.log("Send: HVR1 OFF");
        }
    });
    // HVR2 //
    $('#tb_HV_R2').on('change', function() {
        if ($(this).is(':checked')) {
              switchStatus = $(this).is(':checked');
              websocket.send('HVR2_1');
              smlog("[SENT] HVR2 ON\n");
              // console.log("Send: HVR2 ON");
        }

        else {
             switchStatus = $(this).is(':checked');
             websocket.send('HVR2_0');
             smlog("[SENT] HVR2 OFF\n");
             // console.log("Send: HVR2 OFF");
        }
    });
});


