from PySide6.QtWidgets import QWidget, QVBoxLayout
from PySide6.QtCore import Qt




class GameWidget(QWidget):
    """
    Class representing the base window widget, where the title has already been set
    and a QVBoxLayout is set as the main layout
    """

    gameTitle: str = "The puzzle game"
    fontSizeNormal: int = 20
    fontSizeLarge: int = 30
    fontSizeHuge: int = 50

    alignCenterFlag = Qt.AlignmentFlag.AlignCenter

    def __init__(self):
        super().__init__()
        self.setWindowTitle(self.gameTitle)
        self.resize(0, 0)

        self.mainLayout = QVBoxLayout()
        self.setLayout(self.mainLayout)
    