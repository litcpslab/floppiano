import paho.mqtt.client as mqtt
from .Log import log

initializeMessage: str = "initialize"
initializeMessageAck: str = "initialize_ack"

finishMessage: str = "finished"
finishMessageAck: str= "finished_ack"

class CommunicationManager():
    """
    Class used for communication with the puzzles via MQTT

    The functions:
        onConnect
        onDisconnect
        onMessage 
    will be called when the client connects/disconnects to the broker, or a message is received

    Usage:
        com: CommunicationManager = CommunicationManager()
        com.onConnect = self.onConnect
        com.onDisconnect = lambda code: print(f"{code}")
        com.onMessage = lambda topic, msg: print(f"{topic}: {msg}")

        com.start()
        ...
        com.stop()
    """

    # Parameter which must be set only once
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

        # Callback functionss
        self.onConnect = None
        self.onDisconnect = None
        self.onMessage = None

    def start(self):
        """
        Starts the connection to the broker
        """
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
        """
        Stops the connection to the broker
        """
        self.mqttc.loop_stop()

    def publish(self, topic, message):
        """
        Publish a message at topic
        """
        self.mqttc.publish(topic, message, qos=2)
    
    def subscribe(self, topic):
        """
        Subscribe to a topic
        """
        self.mqttc.subscribe(topic)

    def _onConnect(self, client, userdata, flags, reason_code, properties):
        """
        Internal function which calles self.onConnect()
        """
        # Subscribing in on_connect() means that if we lose the connection and
        # reconnect then subscriptions will be renewed.
        self.onConnect(reason_code)
    
    def _onDisconnect(self, client, userdata, disconnect_flags, reason_code, properties):
        """
        Internal function which calles self.onDisconnect()
        """
        self.mqttc.reconnect()
        self.onDisconnect(reason_code)

    def _onMessage(self, client, userdata, msg):
        """
        Internal function which calles self.onMessage()
        """
        self.onMessage(msg.topic, msg.payload.decode("utf-8"))
    
