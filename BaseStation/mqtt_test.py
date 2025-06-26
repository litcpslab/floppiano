import os
import paho.mqtt.client as mqtt
from dotenv import load_dotenv

def _onConnect(client, userdata, flags, reason_code, properties):
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    print(reason_code)
    mqttc.subscribe("escape/#")

def _onDisconnect(client, userdata, disconnect_flags, reason_code, properties):
    mqttc.reconnect()

def _onMessage(client, userdata, msg):
    print(f"{msg.topic} @ {msg.payload.decode('utf-8')}")

mqttBroker = os.getenv("MQTT_BROKER", "mqtt.eclipseprojects.io")
mqttBroker = "mqtt.eclipseprojects.io"
mqttPort = int(os.getenv("MQTT_PORT", "1883"))
mqttUser = os.getenv("MQTT_USER", "")
mqttPw = os.getenv("MQTT_PW", "")

mqttc = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2, protocol=mqtt.MQTTProtocolVersion.MQTTv5)
#mqttc.username_pw_set(mqttUser, mqttPw)
mqttc.on_connect = _onConnect
mqttc.on_disconnect = _onDisconnect
mqttc.on_message = _onMessage

mqttc.connect(mqttBroker, mqttPort, 60)

mqttc.loop_forever()