from PySide6.QtWidgets import QLabel, QPushButton
from PySide6.QtCore import Qt, QTimer, QThread, Signal
from typing import List

import time

from .GameWidget import GameWidget
from .PuzzleDetailScreen import PuzzleDetailScreen
from .FinishScreen import FinishScreen
from .DebugScreen import DebugScreen

from Classes.CommunicationManager import CommunnicationManager, initializeMessage, initializeMessageAck, finishMessage, finishMessageAck
from Classes.Puzzle import Puzzle, checkAllPuzzlesCompleted, checkAllPuzzlesInitialized
from Classes.Log import log
from Classes.PointTracker import PointTracker

gameTime = 120
startPoints = 10

class PuzzleInitializeThread(QThread):
    def __init__(self, communnicationManager: CommunnicationManager, puzzles: List[Puzzle]):
        super().__init__()
        self.communnicationManager = communnicationManager
        self.puzzles = puzzles
    
    def run(self):
        while True:
            for puzzle in self.puzzles:
                if not puzzle.isInitialized:
                    log(f"Sending activation message to: '{puzzle.name}'")
                    self.communnicationManager.publish(puzzle.mqttTopicGeneral, initializeMessage)
                    time.sleep(0.5)
            time.sleep(3)

class MainScreen(GameWidget):
    timerStartSignal = Signal()
    timerStopSignal = Signal()
    showFinishScreenSignal = Signal()
    setButtonFinishedSignal = Signal(QPushButton, Puzzle)
    setButtonInitializedSignal = Signal(QPushButton, Puzzle)

    def __init__(self, puzzles: List[Puzzle]):
        super().__init__()

        self.puzzles = puzzles

        self.debugScreen = DebugScreen(self.puzzles)
        self.debugScreen.setWindowFlags(Qt.WindowType.WindowStaysOnTopHint)

        self.communnicationManager: CommunnicationManager = CommunnicationManager()
        self.communnicationManager.onConnect = self.onConnect
        self.communnicationManager.onDisconnect = self.onDisconnect
        self.communnicationManager.onMessage = self.onMessage

        self.pointTracker = PointTracker()

        self.puzzleInitializeThread: PuzzleInitializeThread = PuzzleInitializeThread(self.communnicationManager, self.puzzles)
        self.allInitialized: bool = False
        self.secondsLeft: int = gameTime
        self.points: int = 0

        self.timer = QTimer()
        self.timer.timeout.connect(self.updateTime)

        # Signal handling to be thread safe
        self.timerStartSignal.connect(lambda: self.timer.start(1000))
        self.timerStopSignal.connect(lambda: self.timer.stop())
        self.showFinishScreenSignal.connect(self.showFinishScreen)
        self.setButtonFinishedSignal.connect(self.setButtonFinished)
        self.setButtonInitializedSignal.connect(self.setButtonInitialized)

        # UI        
        l = QLabel("Time left")
        self.mainLayout.addWidget(l)

        self.timerLabel = QLabel("Timer")
        self.timerLabel.setStyleSheet(f"font-size: {self.fontSizeHuge}px")
        self.timerLabel.setAlignment(self.alignCenterFlag)
        self.mainLayout.addWidget(self.timerLabel)

        l = QLabel("Points")
        self.mainLayout.addWidget(l)

        self.pointsLabel = QLabel("Points")
        self.pointsLabel.setStyleSheet(f"font-size: {self.fontSizeLarge}px")
        self.pointsLabel.setAlignment(self.alignCenterFlag)
        self.mainLayout.addWidget(self.pointsLabel)

        self.mainLayout.addStretch()

        self.puzzleButtons = [] # Tuple of button and corresponing puzzle
        for puzzle in self.puzzles:
            b = QPushButton(f"Initializing '{puzzle.name}' ...")
            b.setEnabled(False)
            b.clicked.connect(lambda _, p=puzzle: self.showPuzzleDetails(p))
            self.puzzleButtons.append((b, puzzle))
            self.mainLayout.addWidget(b)

    def showEvent(self, event):
        self.communnicationManager.start()
        self.setTime(self.secondsLeft)
        self.setPoints(startPoints, "Start")
        self.puzzleInitializeThread.start()

        event.accept()
    
    def keyPressEvent(self, event):
        if event.key() == Qt.Key.Key_D:
            self.debugScreen.show()
            log("Opening debug widget")
        event.accept()
    
    def showPuzzleDetails(self, puzzle):
        self.puzzleDetailScreen = PuzzleDetailScreen(puzzle, self)
        self.puzzleDetailScreen.show()

    def setPoints(self, points, event=None):
        self.points = points
        
        pointsString = f"{self.points}"
        self.pointsLabel.setText(pointsString)

        self.pointTracker.setPoints(points, event)
    
    def addPoints(self, points, event=None):
        self.setPoints(self.points + points, event)
    
    def subtractPoints(self, points, event=None):
        self.setPoints(self.points - points, event)
    
    def setTime(self, seconds):
        self.secondsLeft = seconds

        timeString = time.strftime('%H:%M:%S', time.gmtime(self.secondsLeft))
        self.timerLabel.setText(timeString)

    def updateTime(self):
        self.setTime(self.secondsLeft - 1)

        if self.secondsLeft < 0:
            self.showFinishScreen()
            return
    
    def showFinishScreen(self):
        self.timerStopSignal.emit()
        self.communnicationManager.stop()
        self.close()

        if self.secondsLeft < 0:
            self.finishScreen = FinishScreen(self.pointTracker, self.puzzles, self.points, 0)
        else:
            self.finishScreen = FinishScreen(self.pointTracker, self.puzzles, self.points, self.secondsLeft)
        self.finishScreen.show()

    # Be carefull
    # This is executed in another thread
    def onMessage(self, topic: str, payload: str):
        button: QPushButton
        puzzle: Puzzle
        for button, puzzle in self.puzzleButtons:

            if topic == puzzle.mqttTopicGeneral:
                if payload == initializeMessageAck:
                    if not puzzle.isCompleted and not puzzle.isInitialized:
                        # Signals for thread safety because layout can be changed here
                        self.setButtonInitializedSignal.emit(button, puzzle)
                        puzzle.isInitialized = True

                elif payload == finishMessage:
                    self.communnicationManager.publish(puzzle.mqttTopicGeneral, finishMessageAck)
                    if not puzzle.isCompleted and self.allInitialized:
                        # Signals for thread safety because layout can be changed here
                        self.setButtonFinishedSignal.emit(button, puzzle)
                        puzzle.isCompleted = True
            
            elif topic == puzzle.mqttTopicPoints:
                if not puzzle.isCompleted and puzzle.isInitialized:
                    try:
                        points = int(payload)
                        self.subtractPoints(points)
                    except:
                        log(f"Cannot convert payload to int: {topic} {payload}")
        
        if not self.allInitialized: # Only do this once
            if checkAllPuzzlesInitialized(self.puzzles):
                log("All puzzles initialized")
                self.puzzleInitializeThread.terminate()
                log("Terminated puzzleInitializeThread")
                self.timerStartSignal.emit()
                log("Sending timer start signal")

                for button, _ in self.puzzleButtons:
                    button.setEnabled(True)

                self.allInitialized = True

        if checkAllPuzzlesCompleted(self.puzzles):
            self.showFinishScreenSignal.emit()
    
    def setButtonInitialized(self, button: QPushButton, puzzle: Puzzle):
        log(f"Initialization done: '{puzzle.name}'")
        button.setText(puzzle.name)

    def setButtonFinished(self, button: QPushButton, puzzle: Puzzle):
        log(f"Puzzle complete: '{puzzle.name}'")
        self.addPoints(puzzle.points, puzzle.name)

        button.setEnabled(False)
        if puzzle.revealAfterFinish is not None:
            self.mainLayout.replaceWidget(button, QLabel(f"Completing {puzzle.name} revealed: {puzzle.revealAfterFinish}"))
            button.deleteLater()
        else:
            button.setText("Completed")
            

    # Be carefull
    # This is executed in another thread
    def onConnect(self, code):
        log(f"Connected with result code: {code} to MQTT Broker")

        for puzzle in self.puzzles:
            self.communnicationManager.subscribe(f"{puzzle.mqttTopic}/#")
    
    # Be carefull
    # This is executed in another thread
    def onDisconnect(self, code):
        log(f"Disconnected with result code: {code}")