# queue-bert
Gets TSOps data and pushes it to Arduino

Uses Python3 to scrape the TSOps webpage for desired data, parses it, and sends it to an Arduino Uno over serial. The Arduino updates an LCD screen with the data and triggers sensory queues (LEDs and a piezo speaker) to keep the user informed on their status.

The code here sources code and libraries that are in the public domain and are NOT my own, and therefore the code here is not intended for sale or distribution by anyone.
