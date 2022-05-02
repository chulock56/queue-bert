#  Python3 code for queue-bert. Initiates serial communication over the specified port to the Arduino, scrapes the TSOps webpage for data, sends data to the Arduino, 
#  receives reply from Arduino and prints to console.
#  
#  A lot of this code was sourced from user Robin2's post on the Arduino forums (https://forum.arduino.cc/index.php?topic=225329.msg1810764#msg1810764)
#  and modified for my use.


#! python3
    # shebang line (for Windows only)


#  Function Definitions

#=====================================

def sendToArduino(sendStr):
    ser.write(sendStr.encode('utf-8'))


#======================================

def recvFromArduino():
    global startMarker, endMarker
    
    ck = ""
    x = "z" # any value that is not an end- or startMarker
    byteCount = -1 # to allow for the fact that the last increment will be one too many
    
    # wait for the start character
    while  ord(x) != startMarker: 
        x = ser.read()
    
    # save data until the end marker is found
    while ord(x) != endMarker:
        if ord(x) != startMarker:
            ck = ck + x.decode("utf-8")
            byteCount += 1
        x = ser.read()
    
    return(ck)


#============================

def waitForArduino():

    # wait until the Arduino sends 'Arduino Ready' - allows time for Arduino reset
    # it also ensures that any bytes left over from a previous message are discarded
    
    global startMarker, endMarker
    
    msg = ""
    while msg.find("Arduino is ready") == -1:

        while ser.inWaiting() == 0:
            pass
        
        msg = recvFromArduino()

        print (msg)
        print ()
        
#======================================

def runControl(td):
    waitingForReply = False

    if waitingForReply == False:
        sendToArduino(td)
        print ("Sent from PC -- " + td)
        waitingForReply = True

    if waitingForReply == True:

        while ser.inWaiting() == 0:
            pass
        
        dataRecvd = recvFromArduino()
        print ("Reply Received  " + dataRecvd)
        waitingForReply = False

        print ("===========")


#======================================
    
def scrapeTsops():
    
    # Scrape queue positions
    try:
        queuePosTableEl = driver.find_element_by_tag_name('app-queue-position')
        queuePosTableText = queuePosTableEl.text
        # print("Position data scraped:\n" + queuePosTableText)

        queuePosTableVals = re.findall(r'\d+|N/A', queuePosTableText) # retrieve queue positions (either int or 'N/A')
    except:
        print("Failed to scrape current queue positions. Continuing...")
        queuePosTableVals = []

    if len(queuePosTableVals) != 0:
        for val in queuePosTableVals:
            if val == 'N/A':
                indx = queuePosTableVals.index(val)
                queuePosTableVals[indx] = 0  # replace any N/A with 0 to avoid datatype conflict on Arduino
        queuePosTableStr = ','.join([str(vals) for vals in queuePosTableVals]) # convert textVals list to string
    else:
        queuePosTableStr = '99,99'


    # Scrape staleness value
    try:
        agentTableEl = driver.find_element_by_tag_name('app-agent-table')
        agentTableText = agentTableEl.text

        myAgentRowMatch = re.search(r'Andrew Chulock ([1-7]?\.?\d\d:\d\d:\d\d|> 1 week)', agentTableText) # find either 9.99:99:99, 99:99:99, or > 1 week
    except:
        print("Failed to scrape current staleness. Continuing...")
        myAgentRowMatch = None

    if myAgentRowMatch != None:
        staleStr = myAgentRowMatch.group(1)
    else:
        staleStr = '9.99:99:99'
    # print("Staleness data scraped + parsed:\n" + staleStr)

    tsopsData = '<' + queuePosTableStr + ',' + staleStr + '>' # append start and end markers
    # print(tsopsData)
    
    return tsopsData


#======================================

# MAIN PROGRAM STARTS HERE

#======================================    

print()
print()

#~~~~~Instantiate Web Driver~~~~~

# NOTE the user must ensure that this directory is available, or edit it to one that is
# checks to see if the Chromedriver is installed and, if not, downloads everything required. 
driverDir = 'C:\\Data\\chromedriver\\'
import pyderman as driver
path = driver.install(browser=driver.chrome, file_directory=driverDir, verbose=True, chmod=True, overwrite=False, version=None)
print('Installed chromedriver to path: %s' % path)

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
# from selenium.webdriver.common.keys import Keys

options = Options()
options.headless = True # change this to change if browser window opens or not 

#~~~~~Instantiate Serial Communication~~~~~

import serial

# NOTE the user must ensure that the serial port and baudrate are correct
# serPort = "/dev/ttyS80"
serPort = "COM6" # if having issue with FileNotFound for the port, try increasing the COM port # (e.g. COM6)
baudRate = 9600

ser = serial.Serial(port=serPort, baudrate=baudRate)
print ("Serial port " + serPort + " opened  Baudrate " + str(baudRate))

startMarker = 60 # '<'
endMarker = 62   # '>'

#~~~~~~~~~~~~~Open Webpage~~~~~~~~~~~~~~~~~

import time

driver = webdriver.Chrome(executable_path=path, options=options)
driver.get('https://<TSOPsHostname>/?skillGroups=FL_English,NOC,Prequeue')

time.sleep(10) # wait a bit for page to load

#~~~~~~~~~~~~~Arduino Initiation~~~~~~~~~~~~~~~~~

waitForArduino() # wait until Arduino confirms it is ready to receive data

#~~~~~Data scrape and write to serial LOOP~~~~~

import re

runInt = 5.0 # how often to run the loop, in seconds
offset = 4.0 # offset from clock, in seconds (i.e. 1:00:00 + offset). Allows time for tsops to update before scraping new values

try:
    while True:
        print('Press Ctrl+C to quit.')
        
        currentTime = time.localtime(time.time())
        print(time.strftime('%Y-%m-%d %H:%M:%S', currentTime))
        
        tsopsData = scrapeTsops()
        runControl(tsopsData) # send data to Arduino
        
        time.sleep((runInt + offset) - time.time() % runInt) # will run loop every X seconds + offset, locked to system clock (i.e. 00:00:06, 00:00:11, etc.)
except KeyboardInterrupt:
    pass

#~~~~~Quit~~~~~

driver.quit() # close all browser windows and exit
ser.close # close serial connection when finished using device

