from PySide6.QtWidgets import  QLabel, QPushButton, QGroupBox, QVBoxLayout

from .GameWidget import GameWidget
from Classes.Puzzle import Puzzle
from Classes.Hint import Hint

class PuzzleDetailScreen(GameWidget):
    def __init__(self, puzzle: Puzzle, mainScreen):
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

        for hint in self.puzzle.hints:
            gb = QGroupBox(hint.name)
            gbl = QVBoxLayout()
            gb.setLayout(gbl)

            if hint.isUsed:
                l = QLabel(hint.text)
                gbl.addWidget(l)
            else:
                b = QPushButton(f"Show hint ({hint.points} points)")
                b.clicked.connect(lambda _, h=hint, gb=gb, gbl=gbl: self.useHint(h, gb, gbl))
                gbl.addWidget(b)
            
            self.mainLayout.addWidget(gb)

    def useHint(self, hint: Hint, groupBox: QGroupBox, groupBoxLayout: QVBoxLayout):
        if self.mainScreen.points - hint.points >= 0:
            hint.isUsed = True
            for child in groupBox.children():
                if type(child) == QPushButton:
                    child.deleteLater()
            
            l = QLabel(hint.text)
            groupBoxLayout.addWidget(l)
            self.mainScreen.subtractPoints(hint.points, "Hint used")