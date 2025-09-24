from datetime import datetime
from pathlib import Path

"""
Logging functions for creating a logfile

Usage:
    from Log import initLogger, log

    initLogger(FILEPATH)
    log("Hello, Logger!")
"""

writeToFile: bool = False
filePath: str = ""

def initLogger(path: Path|None):
    """
    Innitialize the Logger
    Provide a file path object Path (from pathlib.Path) to log messages also to a file and not only the console.

    If no path is provided the messages will only be printed to the console
    """
    global writeToFile, filePath

    if path is None:
        return

    writeToFile = True
    open(path, "w+").close()
    filePath = path


def log(msg):
    """
    Prints the message on the console and depending on the initialization also to a file
    """
    global writeToFile, filePath

    date_time = datetime.now().strftime("%Y/%m/%d %H:%M:%S")
    message = f"{date_time} - {msg}"
    print(message)

    if writeToFile:
        with open(filePath, "a") as f:
            f.write(f"{message}\n")