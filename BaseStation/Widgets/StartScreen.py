from PySide6.QtWidgets import QLabel, QPushButton, QCheckBox, QGroupBox, QVBoxLayout
from PySide6.QtCore import Qt
from typing import List

from .GameWidget import GameWidget
from .MainScreen import MainScreen
from Classes.Log import log
from Classes.Puzzle import Puzzle

class StartScreen(GameWidget):
    def __init__(self, puzzles: List[Puzzle]):
        super().__init__()
        self.puzzles = puzzles

        l = QLabel(f"Welcome to the escape of the \n 'Crazy Professor'")
        l.setStyleSheet(f"font-size: {self.fontSizeLarge}px")
        l.setAlignment(self.alignCenterFlag)
        self.mainLayout.addWidget(l)

        l = QLabel(f"Your task is to complete varios puzzles and escape the labatory of the professor. You can pick which puzzles you wanna play. But be carefull. The more puzzles you play the less time you have for each one. Complete the puzzles to get points. With points you can buy some hints, but they are not cheap. The higher your points are, the better the 'Assosiation of Escapers' will grade you.")
        l.setStyleSheet(f"font-size: {self.fontSizeNormal}px")
        l.setWordWrap(True)
        self.mainLayout.addWidget(l)

        self.mainLayout.addStretch()


        l = QLabel("Select the desired puzzles and then press start")
        self.mainLayout.addWidget(l)

        gb = QGroupBox("Available puzzles")
        gbl = QVBoxLayout()
        gb.setLayout(gbl)
        self.puzzleCheckBoxes = []
        for puzzle in self.puzzles:
            c = QCheckBox(puzzle.name)
            c.setChecked(True)
            self.puzzleCheckBoxes.append((c, puzzle))
            gbl.addWidget(c)
        self.mainLayout.addWidget(gb)

        b = QPushButton("Start")
        b.clicked.connect(self.startGame)
        self.mainLayout.addWidget(b)
    
    def startGame(self):
        c: QCheckBox
        puzzle: Puzzle
        selectedPuzzles: List[Puzzle] = []

        for c, puzzle in self.puzzleCheckBoxes:
            if c.isChecked():
                selectedPuzzles.append(puzzle)
                puzzle.isSelectedForPlay = True
        
        log(f"Selected puzzle: '{[p.name for p in selectedPuzzles]}' for play")

        self.close()

        self.mainScreen = MainScreen(selectedPuzzles)
        self.mainScreen.show()