import paho.mqtt.client as mqtt

from .Log import log

initializeMessage: str = "initialize"
initializeMessageAck: str = "initialize_ack"

finishMessage: str = "finished"
finishMessageAck: str= "finished_ack"

class CommunnicationManager():
    mqttBroker: str | None = None
    mqttPort: int | None = None
    mqttUser: str | None = None
    mqttPw: str | None = None

    def __init__(self):
        self.mqttc = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2, protocol=mqtt.MQTTProtocolVersion.MQTTv5)
        
        if self.mqttUser is not None and self.mqttUser != "":
            self.mqttc.username_pw_set(self.mqttUser, self.mqttPw)

        self.mqttc.on_connect = self._onConnect
        self.mqttc.on_disconnect = self._onDisconnect
        self.mqttc.on_message = self._onMessage

        self.onConnect = None
        self.onDisconnect = None
        self.onMessage = None

    def start(self):
        try:
            returnCode = self.mqttc.connect(self.mqttBroker, self.mqttPort, 60)
        except Exception as ex:
            log(f"Exception occured: {ex}")
            log(f"Could not connect to {self.mqttBroker} @ {self.mqttPort}")
            exit(1)

        if returnCode != 0:
            log("No connection established")
            exit(1)

        log(f"Connection established to {self.mqttBroker} @ {self.mqttPort}")
        self.mqttc.loop_start()
    
    def stop(self):
        self.mqttc.loop_stop()

    def publish(self, topic, message):
        messageInfo = self.mqttc.publish(topic, message, qos=2)
        #messageInfo.wait_for_publish()
    
    def subscribe(self, topic):
        self.mqttc.subscribe(topic)

    def _onConnect(self, client, userdata, flags, reason_code, properties):
        # Subscribing in on_connect() means that if we lose the connection and
        # reconnect then subscriptions will be renewed.
        self.onConnect(reason_code)
    
    def _onDisconnect(self, client, userdata, disconnect_flags, reason_code, properties):
        self.mqttc.reconnect()
        self.onDisconnect(reason_code)

    def _onMessage(self, client, userdata, msg):
        self.onMessage(msg.topic, msg.payload.decode("utf-8"))
    
