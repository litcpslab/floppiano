from PySide6.QtWidgets import  QLabel, QPushButton, QGroupBox, QVBoxLayout

from .GameWidget import GameWidget
from Classes.Puzzle import Puzzle
from Classes.Hint import Hint

class PuzzleDetailScreen(GameWidget):
    """
    Class representing the detail screen of the puzzles, where hints can be bought andd the description of the puzzle can be seen
    """
        
    def __init__(self, puzzle: Puzzle, mainScreen):
        """
        Setup the UI and place the widgets on the screen
        """
        super().__init__()
        self.puzzle: Puzzle = puzzle
        self.mainScreen = mainScreen

        self.nameLabel = QLabel(self.puzzle.name)
        self.nameLabel.setStyleSheet(f"font-size: {self.fontSizeNormal}px")
        self.mainLayout.addWidget(self.nameLabel)

        self.descriptionLabel = QLabel()
        self.descriptionLabel.setWordWrap(True)
        self.mainLayout.addWidget(self.descriptionLabel)

        self.mainLayout.addStretch()

        self.descriptionLabel.setText(self.puzzle.description)

        counter = 1 # Used for senting only the next hint button active
        self.buttons = []

        for hint in self.puzzle.hints:
            gb = QGroupBox(hint.name)
            gbl = QVBoxLayout()
            gb.setLayout(gbl)

            if hint.isUsed:
                l = QLabel(hint.text)
                gbl.addWidget(l)
                self.buttons.append(l)
            else:
                b = QPushButton(f"Show hint ({hint.points} points)")
                self.buttons.append(b)
                if not counter == self.puzzle.nextHint:
                    b.setEnabled(False)
                b.clicked.connect(lambda _, h=hint, gb=gb, gbl=gbl: self.useHint(h, gb, gbl))
                gbl.addWidget(b)
            
            self.mainLayout.addWidget(gb)

            counter += 1

    def useHint(self, hint: Hint, groupBox: QGroupBox, groupBoxLayout: QVBoxLayout):
        """
        Internal function used to display a bought hint and deduct points
        """
        # Only allow to buy a hint if the user has enough points
        if self.mainScreen.points - hint.points >= 0:
            hint.isUsed = True
            self.puzzle.nextHint += 1
            b: QPushButton
            for count, b in enumerate(self.buttons):
                if count+1 == self.puzzle.nextHint:
                    b.setEnabled(True)
                    break

            for child in groupBox.children():
                if type(child) == QPushButton:
                    child.deleteLater()
            
            l = QLabel(hint.text)
            groupBoxLayout.addWidget(l)
            self.mainScreen.subtractPoints(hint.points, f"Hint used\n{self.puzzle.name}")