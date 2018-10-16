//Dexter Carpenter
//Physics Final Project
//Adapted from Elegoo

//Libraries
#include <LiquidCrystal.h> //LCD Lib
#include "SR04.h"          // Ultrasonic Sensor Lib

//Define Pins
const int CalPin = 0;
const int RecPin = 13;
const int LEDPin = 3;
#define ECHO_PIN 4
#define TRIG_PIN 5
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);  //this is defining the pins

/*         Pin Table
 * | Pin A | Pin B | Color  |
 * |   6   |  N/A  |  N\A   |    // Not connected to anything
 * |   7   |  RS   | Green  |
 * |   8   |   E   | Oarnge |
 * |   9   |   D4  |  Blue  |
 * |   10  |   D5  |  Blue  |
 * |   12  |   D6  |  Blue  |
 * |   13  |   D7  |  Blue  |
 * 
 *    Pin A = Arduino
 *    Pin B = Breadboard
 */

//Define Vars
float c = 0;
float clast;
int CalState = 0;
int RecState = 0;
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN); //Setup Ultrasonic
long a;
long alast = 0;                      // Ultrasonic rememberence
long period;                         // In Milliseconds
long starttime;
bool calibrated = false;
bool recording = false;
bool acceptc = false;
int trails = -1;                      // Maybe change the number of starting trials....
//array periods[trials];

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(CalPin, INPUT);
  pinMode(RecPin, INPUT);
  pinMode(LEDPin, OUTPUT);
}

void loop() {
  if ((calibrated == true) and (recording == true)) {
    if ((a <= (c*0.75)) and (a != 0)) {
      period = 0;
      starttime = millis();
      digitalWrite(LEDPin, HIGH);
    } else {
      digitalWrite(LEDPin, LOW);
      period = millis() - starttime;
      //periods[trails + 1] = period;
      lcd.setCursor(10, 1);
      lcd.print(period);
    }
  }
  
  //CALIBRATION
  CalState = digitalRead(CalPin);
  if (CalState == LOW) {
    calibrate();
  }
  
  //RECORDING
  RecState = digitalRead(RecPin);
  //Serial.println(digitalRead(RecPin));
  if (RecState == LOW) {
    if (recording == true) {
      recordingOff();
    } else {
      recordingOn();
    }
  }

  updateLCD();
}

void calibrate() {
  lcd.clear();
  clast = c;
  c=sr04.Distance();
  TestAcceptC();
  if ((acceptc == true) and (recording == false) ) {
    calibrated == true;
  } else if (recording == true) {
    c = clast;
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Cannot Calibrate");
    lcd.setCursor(0, 1); lcd.print("Recording");
    delay(1000);
    lcd.clear();
  } else if (acceptc == false) {
    lcd.clear();
    if (c == 0) {
      lcd.setCursor(0, 0); lcd.print("CAL Can't = 0");
    } else {
      lcd.setCursor(0, 0); lcd.print("CAL Can't be >30");
    }
    lcd.setCursor(0, 1); lcd.print("Try Again");
    delay(2000);
    lcd.clear();
    c = clast;
  }
}

void updateLCD() {
  lcd.setCursor(0, 0);
  a=sr04.Distance();
  if (a != alast) {
    lcd.clear();
    lcd.print(a);
    alast = a;
  }
  
  lcd.setCursor(0, 1); lcd.print("CAL:");
  
  lcd.setCursor(4, 1); lcd.print(c);
  
  lcd.setCursor(14, 0); lcd.print("cm");

  lcd.setCursor(12, 0); lcd.print(recording);

  lcd.setCursor(10, 0); lcd.print(calibrated);
}

void recordingOff() {
  //display all the recorded values
  //wait for a responce
  //clear all the periods
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Recording: OFF");
  delay(1000);
  lcd.clear();
  recording = false;
}

void recordingOn() {
  TestAcceptC();
  if (acceptc == true) {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Recording: ON");
    delay(1000);
    lcd.clear();
    recording = true;
  } else {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Not Calibrated");
    delay(2000);
    lcd.clear();
  }
}

void TestAcceptC() {
  if ((c > 0) and (c < 30)) {
    acceptc = true;
  } else if ((c = 0) or (c >= 30)) {
    acceptc = false;
  }
}





