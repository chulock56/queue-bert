# queue-bert
Gets TSOps data and pushes it to Arduino

Uses Python3 to scrape the TSOps webpage for desired data, parses it, and sends it to an Arduino Uno over serial. The Arduino updates an LCD screen with the data and triggers sensory queues (LEDs and a piezo speaker) to keep the user informed on their status.

The code here sources code and libraries that are in the public domain and are NOT my own, and therefore the code here is not intended for sale or distribution by anyone.

<img src="https://github.com/chulock56/queue-bert/blob/main/ArduinoStuff/queue-bert.jpg"/>

## Contents:
- tsopsRead_NSA / tsopsRead_NoNSA folders contain the .ino (C++) code that should be uploaded to the Arduino.
  - One should be used if you are a part of the NSA queue, and the other if you do not have the NSA skill. 
- tsopsScrape_X.X.py file is the Python3 script that should be run from a PC connected to the Arduino. 
  - NOTE: This file must be modified to: specify the port over which serial communication is initiated with the Arduino; specify a directory to install the Chromium web driver; specify the agent's name for which to scrape TSOps data
- tsopsScrape.bat is provided as a way to run the Python code easily in Windows from a desktop shortcut or from the "Run" dialog. Modify this to point to your Python3 environment directory and location of the tsopsScrape_vX.X.py script.
- ArduinoStuff folder contains information on the Arduino electrical design including diagrams, photos, etc.


## Requirements:
- You must have Python3 installed on your local PC
- You must install the required non-standard Python packages/modules. This can be done using "pip install [moduleName]" from the .../Python/Scripts directory using the command prompt
    - Required packages:
      - pyderman
      - pyserial
      - selenium
- You must download and install the Arduino IDE to upload the .ino code to the Arduino Uno (or knock-off Arduino)
- You need an Arduino Uno or similar microcontroller (there are cheaper options online)
- You need the appropriate hardware (LEDs, LCD screen, piezo speaker, etc. detailed in the parts list)

<img src="https://github.com/chulock56/queue-bert/blob/main/ArduinoStuff/breadboardDiagram.png"/>
