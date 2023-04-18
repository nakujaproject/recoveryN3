import paho.mqtt.client as mqtt

version = '3'
broker = "192.168.1.110"
port = 1883

def on_message(client, userdata, message, tmp=None):
    print(str(message.payload.decode("utf-8")))
    
client = mqtt.Client("P1")
client.on_message = on_message

print("Connecting to broker")
client.connect(broker)

client.publish("house/main-light", "HELLO PAHO") # publish
client.subscribe("house/main-light")
    
client.loop_start()
