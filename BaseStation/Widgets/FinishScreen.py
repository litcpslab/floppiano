from PySide6.QtWidgets import QLabel, QSizePolicy
from typing import List

import time

from .GameWidget import GameWidget
from Classes.Puzzle import Puzzle
from Classes.PointTracker import PointTracker

class FinishScreen(GameWidget):
    def __init__(self, pointTracker: PointTracker, puzzles: List[Puzzle], points, timeLeft):
        """
        Setup the UI and place the widgets on the screen
        """
        super().__init__()
        self.pointTracker = pointTracker
        self.puzzles = puzzles
        self.points = points
        self.timeLeft = timeLeft
        self.completedPuzzles = sum([1 for p in self.puzzles if p.isCompleted])

        self.pointsLabel = QLabel(f"Points: {self.points}")
        self.pointsLabel.setAlignment(self.alignCenterFlag)
        self.pointsLabel.setStyleSheet(f"font-size: {self.fontSizeNormal}px")
        self.mainLayout.addWidget(self.pointsLabel)

        self.timeLeftLabel = QLabel(f"Time left: {time.strftime('%H:%M:%S', time.gmtime(self.timeLeft))}")
        self.timeLeftLabel.setAlignment(self.alignCenterFlag)
        self.timeLeftLabel.setStyleSheet(f"font-size: {self.fontSizeNormal}px")
        self.mainLayout.addWidget(self.timeLeftLabel)

        self.completedPuzzlesLabel = QLabel(f"Completed puzzles: {self.completedPuzzles}")
        self.completedPuzzlesLabel.setAlignment(self.alignCenterFlag)
        self.completedPuzzlesLabel.setStyleSheet(f"font-size: {self.fontSizeNormal}px")
        self.mainLayout.addWidget(self.completedPuzzlesLabel)

        plot = self.pointTracker.getPlot()
        plot.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        self.mainLayout.addWidget(plot)

        self.finalScoreLabel = QLabel(f"Final score: {self.points}")
        self.finalScoreLabel.setAlignment(self.alignCenterFlag)
        self.finalScoreLabel.setStyleSheet(f"font-size: {self.fontSizeLarge}px")
        self.mainLayout.addWidget(self.finalScoreLabel)