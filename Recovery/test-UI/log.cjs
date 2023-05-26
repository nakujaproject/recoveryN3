const mqtt = require('paho-mqtt');
global.WebSocket = require('ws');
let fs = require('fs');

client = new mqtt.Client("127.0.0.1", 1885, "logger");
 
// set callback handlers
client.onConnectionLost = onConnectionLost;
client.onMessageArrived = onMessageArrived;
 
// connect the client
client.connect({onSuccess:onConnect});

//create logging file name based on epoch
let date = new Date();
let epoch = date.getTime();
let filename = 'logs.csv';

// called when the client connects
function onConnect() {
    // Once a connection has been made, make a subscription and send a message.
    console.log("onConnect");
    client.subscribe("piston/force");
    filename = 'logs/log_'+epoch.toString().slice(6)+'.csv';
}
   
// called when the client loses its connection
function onConnectionLost(responseObject) {
    if (responseObject.errorCode !== 0) {
        console.log("onConnectionLost:"+responseObject.errorMessage);
    }
}

// called when a message arrives
function onMessageArrived(message) {
    // console.log("onMessageArrived:");
    try{
        fs.appendFile(filename,message.payloadString+'\n',(e)=>{
            if(e){
                console.log(e);
            }
            console.log(message.payloadString);
        });
    } catch(err){
        console.log(err);
    }
}