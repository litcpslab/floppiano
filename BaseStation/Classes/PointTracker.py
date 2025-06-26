import os
from datetime import datetime
from PySide6.QtWidgets import QApplication
from PySide6.QtGui import QPalette

os.environ["QT_API"] = "PySide6"

import matplotlib.dates as md
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg
from matplotlib.figure import Figure


class MatplotlibCanvas(FigureCanvasQTAgg):
    def __init__(self, width, height, dpi):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)

        fig.set_facecolor('none')
        fig.tight_layout()

        super().__init__(fig)

        self.setStyleSheet("background: transparent")

class PointTracker():
    def __init__(self):
        self.timestamps = []
        self.points = []
        self.events = []

        self.mlptCanvas = MatplotlibCanvas(width=5, height=4, dpi=100)
        self.formatter = md.DateFormatter('%H:%M:%S')

        palette = QApplication.palette()
        self.textColor = palette.color(QPalette.ColorRole.WindowText).name()
        self.backgroundColor = palette.color(QPalette.ColorRole.Window).name()
    
    def setPoints(self, points, event):
        self.timestamps.append(datetime.now())
        self.points.append(points)
        self.events.append(event)

        datenums = md.date2num(self.timestamps)

        self.mlptCanvas.axes.clear()
        self.mlptCanvas.axes.plot(datenums,self.points)

        for x, y, text in zip(datenums, self.points, self.events):
            if text is not None:
                self.mlptCanvas.axes.text(x=x, y=y, s=text, horizontalalignment="center", color=self.textColor, bbox=dict(boxstyle="round", facecolor=self.backgroundColor, edgecolor=self.textColor))

        self.mlptCanvas.draw()

    def getPlot(self):
        self.mlptCanvas.axes.xaxis.set_major_formatter(self.formatter)
        self.mlptCanvas.axes.set_facecolor('none')
        self.mlptCanvas.axes.tick_params(colors=self.textColor)
        self.mlptCanvas.axes.spines[:].set_color(self.textColor)
        self.mlptCanvas.axes.yaxis.label.set_color(self.textColor)
        self.mlptCanvas.axes.xaxis.label.set_color(self.textColor)
        self.mlptCanvas.axes.title.set_color(self.textColor)
    
        return self.mlptCanvas