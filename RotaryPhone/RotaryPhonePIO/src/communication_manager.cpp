#include "communication_manager.h"

WiFiClient espClient;
PubSubClient client(espClient);
extern bool active;

void client_reconnect() {
  String id = "esp32-client-" + String(WiFi.macAddress());
  const char* client_id = id.c_str();

  bool return_value;
  while (!client.connected()) {
    #if MQTT_NEEDS_USER == 1
      return_value = client.connect(client_id, mqtt_username, mqtt_password);
    #else
      return_value = client.connect(client_id);
    #endif

    if (return_value) {
        #if DEBUG
          Serial.print("Connected to MQTT broker: ");
          Serial.println(mqtt_broker);
        #endif
    } else {
        #if DEBUG
          Serial.print("Connection to MQTT broker failed with state: ");
          Serial.println(client.state());
          Serial.println("Trying to reconnect in 2 sec");
        #endif
        delay(2000);
    }
  }

  return_value = client.subscribe(mqtt_topic_general);
  #if DEBUG
    if (return_value) {
      Serial.print("Subscribed succesfully to: ");
      Serial.println(mqtt_topic_general);
    }
  #endif
}

void setup_communication() {
  delay(10);

  #if DEBUG
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
  #endif

  // Setup WiFi parameter
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  //WiFi.setMinSecurity(WIFI_AUTH_WPA_PSK); 

  // Start connection
  WiFi.begin(ssid, password);

  while (!WiFi.isConnected()) {
      delay(200);
      #if DEBUG
        Serial.print(".");
      #endif
  }

  #if DEBUG
    Serial.println();
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  #endif

  // Setup MQTT parameter
  client.setKeepAlive(90);
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(mqtt_callback);

  // Use defined function to connect to MQTT server
  client_reconnect();
  // Publish a hello message
  client.publish(mqtt_topic_general, "I am online");
}

void loop_communication() {
  if (!client.connected()) {
    #if DEBUG
      Serial.println("Connection lost to MQTT server!");
    #endif
    client_reconnect();
  }
  client.loop();
}

void mqtt_callback(char *topic, byte *payload, unsigned int length) {
  String message_str = String("");
  String topic_str = String(topic);

  for (int i = 0; i < length; i++) {
    message_str += (char) payload[i];
  }

  #if DEBUG
    Serial.print("Message arrived in topic: ");
    Serial.println(topic_str);
    Serial.print("Message: ");
    Serial.println(message_str);
    Serial.println();
  #endif

  if (topic_str == String(mqtt_topic_general)) {
    if (message_str == String("initialize")) {
      #if DEBUG
        Serial.println("Initialize message received");
      #endif
      client.publish(mqtt_topic_general, "initialize_ack");
      active = true;
    }
  }
}

void publish_done() {
    if (client.connected()) {
        bool result = client.publish(mqtt_topic_general, "finished");
        active = false;
        #if DEBUG
          if (result) {
              Serial.println("Published 'finished' message");
          } else {
              Serial.println("Failed to publish 'finished'");
          }
        #endif  
    }
    #if DEBUG
      else {
          Serial.println("MQTT not connected: cannot publish 'finished' message");
      }
    #endif
}

