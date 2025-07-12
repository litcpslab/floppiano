from PySide6.QtWidgets import QLabel, QPushButton, QCheckBox, QGroupBox, QVBoxLayout
from PySide6.QtCore import Qt
from typing import List

from .GameWidget import GameWidget
from .MainScreen import MainScreen
from Classes.Log import log
from Classes.Puzzle import Puzzle

story = """
Prof. L. Toiz took over the lecture for Nonlinear electrical systems (NES) two years ago, since then not a single student passed its crazy difficult final exam. You are a group of students who tried it the fifth time this morning, the last possible time before you would be exmatriculated forever.

You are all sure that you will fail again, as this exam was even more difficult than the previous ones. As a last resort you decide to break into the Professors office to steal your written exams, so that they do not get graded and you can save your otherwise quite successful academic career.

Everyday precisely at 11:40 Prof. L. Toiz leaves his office for lunch and returns at 12:20. At 11:45 you enter the office unnoticed and are welcomed by a messy workplace filled with a vast amount of different embedded devices and gadgets. As soon as you close the door behind you the security system goes active. After rummaging through all the loose papers on the desk, you still could not find your exams, they have to be hidden somewhere else in this room.
 
Your eyes fall upon a strange boxlike contraption with nine numbered buttons on the table. There is also a box with a screen on top that catches your attention. It seems like they are some puzzles, maybe they lead to your exams? 

Can you find them before the Prof. returns...
"""

class StartScreen(GameWidget):
    """
    Class representing the start screen widget, where the puzzles can be selected which are beeing played
    """
        
    def __init__(self, puzzles: List[Puzzle]):
        """
        Setup the UI and place the widgets on the screen
        """
        super().__init__()
        self.puzzles = puzzles

        l = QLabel(f"Welcome to the escape from the\n'Crazy Professor'")
        l.setStyleSheet(f"font-size: {self.fontSizeLarge}px")
        l.setAlignment(self.alignCenterFlag)
        self.mainLayout.addWidget(l)

        l = QLabel(story)
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
        """
        Creates the main screen object and shows it and changes the isSelectedForPlay field of the selected puzzles
        """
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