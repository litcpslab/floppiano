import json
from typing import List
from pathlib import Path

from .Hint import Hint

class Puzzle():
    """
    Class representing a Puzzle
    """
    def __init__(self):
        # From JSON
        self.name: str = ""
        self.description: str = ""
        self.points: int = 0
        self.hints: List[Hint] = []
        self.mqttTopic: str = ""
        self.revealAfterFinish: str | None = None

        # Internal states
        self.isCompleted: bool = False
        self.isInitialized: bool = False
        self.isSelectedForPlay: bool = False
        self.nextHint = 1

        self.mqttTopicGeneral: str = ""
        self.mqttTopicPoints: str = ""
    
def checkAllPuzzlesCompleted(puzzleList: List[Puzzle]):
    """
    Pass a lit of puzzles and returns true if every puzzle is completed
    """
    puzzle: Puzzle
    for puzzle in puzzleList:
        if not puzzle.isCompleted:
            return False
    return True

def checkAllPuzzlesInitialized(puzzleList: List[Puzzle]):
    """
    Pass a lit of puzzles and returns true if every puzzle is initialized
    """
    puzzle: Puzzle
    for puzzle in puzzleList:
        if not puzzle.isInitialized:
            return False
    return True

def getPuzzleFromFile(filePath: Path):
    """
    Provide a file path to a json file and returns the data from the json file as a puzzle object 
    """
    data: dict
    with open(filePath) as f:
        data = json.load(f)
    
    p = Puzzle()
    p.name = data["name"]
    p.description = data["description"]
    p.points = data["points"]

    if "reveal_after_finish" in data.keys():
        p.revealAfterFinish = data["reveal_after_finish"]

    p.mqttTopic = data["mqtt"]["topic"]
    p.mqttTopicGeneral = p.mqttTopic + "/general"
    p.mqttTopicPoints = p.mqttTopic + "/points"

    for hint in data["hints"]:
        h = Hint()
        h.name = hint["name"]
        h.text = hint["text"]
        h.points = hint["points"]

        p.hints.append(h)
    return p

def getAllPuzzlesFromFolder(folderPath: Path):
    """
    Provide a folder path where only json files are located and returns the data from the json files as a list of puzzles 
    """
    if not folderPath.exists():
        raise Exception(f"Path '{folderPath.absolute()}' does not exist")
    puzzles = []
    for file in folderPath.iterdir():
        p = getPuzzleFromFile(file)
        puzzles.append(p)
    
    return sorted(puzzles, key=lambda p: p.name)