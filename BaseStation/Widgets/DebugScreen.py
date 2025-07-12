from PySide6.QtWidgets import QWidget, QLabel, QPushButton, QHBoxLayout, QVBoxLayout, QGroupBox, QLineEdit, QListWidget, QListWidgetItem
from typing import List

from .GameWidget import GameWidget
from Classes.CommunicationManager import CommunnicationManager, initializeMessage, initializeMessageAck, finishMessage, finishMessageAck
from Classes.Puzzle import Puzzle
from Classes.Log import log


class DebugScreen(GameWidget):
    def __init__(self, puzzles: List[Puzzle]):
        """
        Setup the UI and place the widgets on the screen
        """
        super().__init__()
        self.puzzles = puzzles

        self.communnicationManager: CommunnicationManager = CommunnicationManager()
        self.communnicationManager.onConnect = self.onConnect
        self.communnicationManager.onDisconnect = self.onDisconnect
        self.communnicationManager.onMessage = self.onMessage
        self.communnicationManager.start()

        # UI
        self.puzzleGroupBoxes = []
        puzzle: Puzzle
        for puzzle in self.puzzles:
            gb = QGroupBox(puzzle.name)
            gb.setEnabled(False)
            self.puzzleGroupBoxes.append((gb, puzzle))

            hl = QHBoxLayout()
            gb.setLayout(hl)

            b = QPushButton("ACK initialize")
            b.clicked.connect(lambda _, topic=puzzle.mqttTopicGeneral: self.sendMessage(topic, initializeMessageAck))
            hl.addWidget(b)

            b = QPushButton("Finish")
            b.clicked.connect(lambda _, topic=puzzle.mqttTopicGeneral: self.sendMessage(topic, finishMessage))
            hl.addWidget(b)

            tb = QLineEdit()
            hl.addWidget(tb)

            b = QPushButton("Points")
            b.clicked.connect(lambda _, topic=puzzle.mqttTopicPoints, tb=tb: self.sendMessage(topic, int(tb.text())))
            hl.addWidget(b)

            self.mainLayout.addWidget(gb)
        
        self.initializeAllButton = QPushButton("ACK initialize all")
        self.initializeAllButton.clicked.connect(self.initializeAll)
        self.mainLayout.addWidget(self.initializeAllButton)
        
        self.mainLayout.addStretch()
        
        self.logBox = QListWidget()
        self.mainLayout.addWidget(self.logBox)
    
    def initializeAll(self):
        """
        Internal function used to send initialize messages for all puzzles
        """
        for puzzle in self.puzzles:
            self.sendMessage(puzzle.mqttTopicGeneral, initializeMessageAck)
    
    def sendMessage(self, topic, message):
        self.communnicationManager.publish(topic, message)

    def onMessage(self, topic: str, payload: str):
        """
        Internal function needed for the CommunicationManager
        """
        groupBox: QGroupBox
        puzzle: Puzzle
        self.logBox.insertItem(0, f"{topic} - {payload}")
        for groupBox, puzzle in self.puzzleGroupBoxes:
            if topic == puzzle.mqttTopicGeneral:
                if payload == initializeMessage:
                    groupBox.setEnabled(True)

    def onConnect(self, code):
        """
        Internal function needed for the CommunicationManager
        """
        log(f"Connected with result code: {code} to MQTT Broker")
        for puzzle in self.puzzles:
            self.communnicationManager.subscribe(f"{puzzle.mqttTopic}/#")
    
    def onDisconnect(self, code):
        """
        Internal function needed for the CommunicationManager
        """
        log(f"Disconnected with result code: {code}")
