# How the escape room works
The base station provides the story and the functionalities to view the description of the puzzles and to by hints. Hints can be bought using points. After the time runs out, or when every puzzle is finished, the escape room ends and the player is granted the final points.

# Base Station

This is a python project which implements the UI for the escape room.

PySide6 is used to display graphical components on the display.

Via MQTT the different puzzles can communicate with the base station. For this, all available puzzles must be described in the `Puzzles` folder by `.json` files. The naming of the `.json` files can be arbitrary chosen.

## Python environment

For creating the virtual environment please note that depending on your python installation type `python, python3, python3.12, ...`.

## Installation Mac Os

```
python3 -m venv env
source env/bin/activate
pip install -r requirements_MacOs.txt
```

## Installation Raspberry Pi Os
Because the Raspberry Pi Os does not support the latest PySide 6 libraries a seperate requirements file is needed.
```
python3 -m venv env
source env/bin/activate
pip install -r requirements_Raspberry.txt
```

## Runing the application

For running the application navigate to the folder where `main.py` is located. Then activate the virtual environment via `source env/bin/activate`. After that just run the file via `python main.py`.

### Set up env variables

In the `main.py` environtment variables are read, which specify the MQTT broker and port and also the username and password.

To succesfully run the script create a new file called `.env` and put following text inside.

```
MQTT_BROKER=address
MQTT_PORT=1883
MQTT_USER=username
MQTT_PW=password
```

## Log file

After each start of the application, a log file (`log.txt`) is created, where the application logs the current events like when a puzzle sends a finish command.

## Code structure

### UI files

All files which are responsible for showing the UI on the screen, are located in `./Widgets`.
The `GameWidget` represents a base class, which all windows will inerite from, where the game title and the general layout of the UI is set

There are 5 different screens available
1. StartScreen
Here the user can select which puzzles to play and gets an explanation how the escape room wors
1. MainScreen
Here the remaining time as well as the points are displayed. By clicking on a puzzle the PuzzleDetailScreen will open
1. PuzzleDetailScreen
Here a description of the puzzle is shown which is defined in the config json files. The user can also buy hints with the gathered points
1. FinishScreen
Here a plot is shown, where the user can see, at which point in time a puzzle was completed or when a hint was bought
1. DebugScreen
This is for debugging and testing purposes, to simulate the communication with the puzzles

### Helper classes

In the folder `./Classes` several helper classes are defined like the structure of puzzles and hints as well as a logger class and a class for communication via MQTT.

# Change Story

To quickly change the story which is shown int the start screen, change the text in the `story`variable in the top of `StartScreen.py`.