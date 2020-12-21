/* data scraped := [int ENGPOS, int NOCPOS, int STALENESS, int INCALLS, int OUTCALLS] 

NOTES: 
	- int xxxPOS can also be N/A, so need to account for this in Python code
	- int STALENESS will be converted from days.hours:min:sec to seconds in Python code.
		- It can also be '>1 week', so need to account for that in Python code
*/

#include <LiquidCrystal.h>

// Initialize variables
const byte numLEDs = 2;
byte ledPin[numLEDs] = {8, 9};

const int notes[] = {131, 196, 262, 392}; // C3, G3, C4, G4 note frequencies for piezo
const int piezoPin = 7;

const byte buffSize = 40;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;

char messageFromPC[buffSize] = {0};
int EngPos = 99;
int newEngPos = 99;
int NocPos = 99;
int newNocPos = 99;
char Stale[buffSize] = {0};
char newStale[buffSize] = {0};

int severity = 1;

unsigned long curMillis;

// initialize LCD library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Initialize Arduino
void setup() {
  Serial.begin(9600);
  
    // flash LEDs so we know we are alive
  for (byte n = 0; n < numLEDs; n++) {
     pinMode(ledPin[n], OUTPUT);
     digitalWrite(ledPin[n], HIGH);
  }
  delay(500); // delay() is OK in setup as it only happens once
  
  for (byte n = 0; n < numLEDs; n++) {
     digitalWrite(ledPin[n], LOW);
  }
  
    // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  // Print constant parts to the LCD
  lcd.print("ENG:");
  lcd.setCursor(9,0);
  lcd.print("NOC:");
  lcd.setCursor(0,1);
  lcd.print("STLE:"); // staleness
  
    // tell the PC we are ready
  Serial.println("<Arduino is ready>");
}

//=============

void loop() {
  curMillis = millis();
  getDataFromPC();
  replyToPC();
  updateLCD();
}

//=============

void getDataFromPC() {

    // receive data from PC and save it into inputBuffer
    
  if(Serial.available() > 0) {

    char x = Serial.read();

      // the order of these IF clauses is significant
      
    if (x == endMarker) {
      readInProgress = false;
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      parseData();
    }
    
    if(readInProgress) {
      inputBuffer[bytesRecvd] = x;
      bytesRecvd ++;
      if (bytesRecvd == buffSize) {
        bytesRecvd = buffSize - 1;
      }
    }

    if (x == startMarker) { 
      bytesRecvd = 0; 
      readInProgress = true;
    }
  }
}

//=============
 
void parseData() {

    // split the data into its parts
    
  char * strtokIndx; // this is used by strtok() as an index
  
  strtokIndx = strtok(inputBuffer, ","); // get the first part - English position
  newEngPos = atoi(strtokIndx);          // convert this part to integer
  
  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  newNocPos = atoi(strtokIndx);   // convert this part to an integer
  
  strtokIndx = strtok(NULL, ",");    // this continues where the previous call left off
  strcpy(newStale, strtokIndx); // copy it to stale

}

//=============

void replyToPC() {

  if (newDataFromPC) {
    newDataFromPC = false;
    Serial.print("<ENG ");
    Serial.print(newEngPos);
    Serial.print(" NOC ");
    Serial.print(newNocPos);
    Serial.print(" Staleness ");
    Serial.print(newStale);
    Serial.print(" RunTime ");
    Serial.print(curMillis >> 9); // divide by 512 is approx = half-seconds
    Serial.println(">");
  }
}

//============

void updateLCD() {

  if (newEngPos != EngPos) {
    // print new ENG position to screen
    lcd.setCursor(4,0);
    lcd.print("  "); // clear previous entry
    lcd.setCursor(4,0);
    lcd.print(newEngPos);

    EngPos = newEngPos;
  }
  
  if (newNocPos != NocPos) {
    // print new NOC position to screen
    lcd.setCursor(13,0);
    lcd.print("  "); // clear previous entry
    lcd.setCursor(13,0);
    lcd.print(newNocPos);

    NocPos = newNocPos;
  }
  
  if (strcmp(newStale, Stale) != 0) {
    // print new staleness to screen
    lcd.setCursor(6,1);
    lcd.print("          "); // clear previous entry
    lcd.setCursor(6,1);
    lcd.print(newStale);

    strcpy(Stale, newStale); 
  }
  
  // flash lights and make sound, if necessary
  audVisAlert();
}

//=============

void audVisAlert() {
	if (newEngPos > EngPos) {
		  // flash green and play first note
		digitalWrite(ledPin[0], HIGH);
		tone(piezoPin, notes[2]);
      delay(50);
    digitalWrite(ledPin[0], LOW);
      delay(25);

      // flash green and play second note
    digitalWrite(ledPin[0], HIGH);
    tone(piezoPin, notes[3]);
      delay(100);

      // turn off light and stop playing notes
    noTone(piezoPin); 
		digitalWrite(ledPin[0], LOW);
	}
 
	else if ((newEngPos < EngPos) && (newEngPos != 0)) {
		int posDiff = EngPos - newEngPos;
   
    if (posDiff > 9) severity = 4;
    else if (posDiff > 4) severity = 3;
    else if (posDiff > 1) severity = 2;
    else severity = 1;

      // flash and beep number of times according to severity
    for (int i = 0; i < severity; i++) {
  		  // flash red and play first note
  		digitalWrite(ledPin[1], HIGH);
      tone(piezoPin, notes[1]);
        delay(50);
      digitalWrite(ledPin[1], LOW);
        delay(25);
        
        // flash red and play second note
      digitalWrite(ledPin[1], HIGH);
      tone(piezoPin, notes[0]);
        delay(100);
        
        // turn off light and stop playing notes
      noTone(piezoPin); 
      digitalWrite(ledPin[1], LOW);
    }
	}
  // else position did not change; do nothing
}
