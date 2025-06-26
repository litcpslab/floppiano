from datetime import datetime
from pathlib import Path

writeToFile: bool = False
filePath: str = ""

def initLogger(path: Path|None):
    global writeToFile, filePath

    if path is None:
        return

    writeToFile = True
    open(path, "w+").close()
    filePath = path


def log(msg):
    global writeToFile, filePath

    date_time = datetime.now().strftime("%Y/%m/%d %H:%M:%S")
    message = f"{date_time} - {msg}"
    print(message)

    if writeToFile:
        with open(filePath, "a") as f:
            f.write(f"{message}\n")