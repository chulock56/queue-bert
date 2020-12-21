# queue-bert
Gets TSOps data and pushes it to Arduino

Uses Python3 to scrape the TSOps webpage for desired data, parses it, and sends it to an Arduino Uno over serial. The Arduino updates an LCD screen with the data and triggers sensory queues (LEDs and a piezo speaker) to keep the user informed on their status.

The code here sources code and libraries that are in the public domain and are NOT my own, and therefore the code here is not intended for sale or distribution by anyone.

## Contents:
- tsopsRead folder contains the .ino (C++) code that should be uploaded to the Arduino.
- tsopsScrape_v1.X.py file is the Python3 script that should be run from a PC connected to the Arduino. NOTE: This file must be modified to specify the port over serial communication is initiated with the Arduino.
- tsopsScrape.bat is provided as a way to run the Python code easily in Windows from a desktop shortcut or from the "Run" dialog. Modify this to point to your Python3 environment directory and location of the tsopsScrape_v1.x.py script.
- ArduinoStuff folder contains information on the Arduino electrical design including diagrams, photos, etc.
