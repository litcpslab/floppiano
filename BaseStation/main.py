import sys
import os
from pathlib import Path
from dotenv import load_dotenv

from Classes.Puzzle import getAllPuzzlesFromFolder
from Classes.Log import log, initLogger
from PySide6.QtWidgets import QApplication
from Widgets.StartScreen import StartScreen
from Classes.CommunicationManager import CommunicationManager

initLogger(Path(".", "log.txt"))
log("Starting Application ...")

log("Loading environment variables")
load_dotenv()
CommunicationManager.mqttBroker = os.getenv("MQTT_BROKER", "mqtt.eclipseprojects.io")
CommunicationManager.mqttPort = int(os.getenv("MQTT_PORT", "1883"))
CommunicationManager.mqttUser = os.getenv("MQTT_USER", "")
CommunicationManager.mqttPw = os.getenv("MQTT_PW", "")

log("Getting available puzzles from folder")
puzzles = getAllPuzzlesFromFolder(Path(".", "Puzzles"))

log("Creating QApplication")
app = QApplication(sys.argv)

startScreen = StartScreen(puzzles)
startScreen.show()

app.exec() # UI loop, lines after this are only executed when all windows are closed
log("Leaving Application ...")