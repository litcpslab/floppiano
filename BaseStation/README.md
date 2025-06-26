# Base Station

This is a python project which implements the UI for the escape room.

PySide6 is used to display graphical components on the display.

Via MQTT the different puzzles can communicate with the base station. For this, all available puzzles must be described in the `Puzzles` folder by `.json` files. The naming of the `.json` files can be arbitrary chosen.

## Installation Mac Os

For creating the virtual environment please note that depending on your python installation type
`python, python3, python3.12, ...`

```
python3 -m venv env
source env/bin/activate
pip install -r requirements_MacOs.txt
```

## Installation Raspberry Pi Os

For creating the virtual environment please note that depending on your python installation type
`python, python3, python3.12, ...`

```
python3 -m venv env
source env/bin/activate
pip install -r requirements_MacOs.txt
```

## Runing the application

For running the application navigate to the folder where `main.py` is located. Then activate the virtual environment via `source env/bin/activate`. After that just run the file via `python main.py`.

### Set up env variables

In the `main.py` environtment variables are read, which specify the MQTT broker and port and also the username and password.

To succesfully run the script create a new file called `.env` and put follwoing text inside it

```
MQTT_BROKER=address
MQTT_PORT=1883
MQTT_USER=username
MQTT_PW=password
```