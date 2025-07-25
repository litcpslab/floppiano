import paho.mqtt.client as mqtt
import time
import uuid
from dotenv import load_dotenv
import os

load_dotenv()  # Load .env file

# Configuration (replace with your values)
MQTT_BROKER = os.getenv("MQTT_BROKER", "localhost")
MQTT_PORT = int(os.getenv("MQTT_PORT", 1883))
MQTT_TOPIC_GENERAL = os.getenv("MQTT_TOPIC_GENERAL", "general")
MQTT_TOPIC_POINTS = os.getenv("MQTT_TOPIC_POINTS", "points")
MQTT_USERNAME = os.getenv("MQTT_USERNAME")
MQTT_PASSWORD = os.getenv("MQTT_PASSWORD")
MQTT_NEEDS_USER = True  # Set to False if you don't need auth
DEBUG = True

# Generate unique client ID (like esp32-client-MAC)
client_id = f"raspi-client-{uuid.uuid4().hex[:8]}"

client = mqtt.Client(client_id)

if MQTT_NEEDS_USER:
    client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)

# Callback function
def on_message(client, userdata, msg):
    topic_str = msg.topic
    message_str = msg.payload.decode()

    if DEBUG:
        print(f"[MQTT] Message received on topic: {topic_str}")
        print(f"[MQTT] Message: {message_str}")

    if topic_str == MQTT_TOPIC_GENERAL:
        if message_str == "initialize":
            if DEBUG:
                print("[MQTT] Initialize message received")
            client.publish(MQTT_TOPIC_GENERAL, "initialize_ack")
            # Do something here...

# Setup MQTT connection
def setup_communication():
    if DEBUG:
        print(f"[MQTT] Connecting to broker {MQTT_BROKER}...")

    client.on_message = on_message
    client.reconnect_delay_set(min_delay=2, max_delay=10)
    client.keep_alive = 90
    client.connect(MQTT_BROKER, MQTT_PORT, keepalive=90)

    client.loop_start()

    while not client.is_connected():
        if DEBUG:
            print("[MQTT] Waiting for connection...")
        time.sleep(1)

    client.subscribe(MQTT_TOPIC_GENERAL)

    if DEBUG:
        print(f"[MQTT] Subscribed to {MQTT_TOPIC_GENERAL}")

    client.publish(MQTT_TOPIC_GENERAL, "I am online")

# Periodically check MQTT connection
def loop_communication():
    if not client.is_connected():
        if DEBUG:
            print("[MQTT] Disconnected. Reconnecting...")
        try:
            client.reconnect()
        except Exception as e:
            print(f"[MQTT] Reconnect failed: {e}")
    client.loop()

def start():
    if DEBUG:
        print("[MQTT] Starting communication loop")
    setup_communication()

def stop():
    if DEBUG:
        print("[MQTT] Stopping communication")
    client.loop_stop()
    client.disconnect()

def publish_message(topic, message):
    if DEBUG:
        print(f"[MQTT] Publishing message to {topic}: {message}")
    client.publish(topic, message)

def publish_finished():
    if DEBUG:
        print("[MQTT] Publishing finished message")
    client.publish(MQTT_TOPIC_GENERAL, "finished")

# Example usage
if __name__ == "__main__":
    setup_communication()
    try:
        while True:
            loop_communication()
            time.sleep(1)
    except KeyboardInterrupt:
        print("[MQTT] Shutting down")
        client.loop_stop()
        client.disconnect()
