class Hint():
    """
    Class representing a Hint for a Puzzle
    """
    def __init__(self):
        self.name: str = None
        self.text: str = None
        self.points: int = None
        self.isUsed: bool = False