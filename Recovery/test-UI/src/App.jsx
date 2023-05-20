import { useState } from 'react'
import MQTT from 'paho-mqtt'
import nakuja from '/nakuja_logo.png'
import setting from './assets/setting.svg';

let socket = new WebSocket("ws://192.168.100.2:8001");
socket.onopen = function(e) {
  alert("[open] Connection established");
};
let client = new MQTT.Client("127.0.0.1", 1885, "dashboard");
//called when client connects
let onConnect = () => {
  console.log("connected");
  client.subscribe("piston/force");
}
// connect the client
client.connect({
  onSuccess:onConnect,
  keepAliveInterval: 3600,
});
export default function App() {
  const [force, setForce] = useState(0)
  let [connection, setConnection] = useState(false)
  let [time, setTime] = useState(5);
  socket.onmessage = function(event) {
    console.log(event);
    document.getElementById("root").innerHTML = event.data;
    alert(`[message] Data received from server: ${event.data}`);
}
// called when the client loses its connection
let onConnectionLost = (responseObject) => {
  if (responseObject.errorCode !== 0) {
  console.log("onConnectionLost:"+responseObject.errorMessage);
  }
  // connect the client
  client.connect({
    onSuccess:onConnect,
    keepAliveInterval: 3600,
  });
}

// called when a message arrives
let onMessageArrived = (message) => {
  console.log("onMessageArrived:");
  setForce(message.payloadString);
}

// set callback handlers
client.onConnectionLost = onConnectionLost;
client.onMessageArrived = onMessageArrived;
//send message
let sendMessage = (e,command) => {
  e.preventDefault();
  // socket.send(JSON.stringify({message:command}));
  let message = new MQTT.Message(command)
  message.destinationName = "piston/activate";
  client.send(message);
}

  return (
    <div className='container mx-auto tracking-wide'>
      <div className='header flex justify-evenly items-center my-4'>
        <img className='w-2/12 md:w-[5%]' src={nakuja} alt=""/>
        <div>{connection?"Connected":"Connecting..."}</div>
        <button onClick={e=>{document.getElementById('settings').style.visibility='visible'}}><img src={setting} className=""/></button>
      </div>
      <div className='text-center mt-32 md:mt-64'><span className='text-4xl'>{force} </span>N</div>
      <div className='countdown text-center text-4xl my-8 md:my-24'>{time}</div>
      <button className='bg-green-500 text-lg font-semibold tracking-wide py-4 md:w-1/4 w-3/4 my-8 rounded-full block mx-auto' onClick={e=>sendMessage(e,'FIRE')}>IGNITE</button>
    </div>
  )
}