//Dexter Carpenter
//IB Computer Science IA
//Adapted from elegoo.com
//DATE: 12/4/18
//Arduino MEGA 2560

//Libraries
#include <LiquidCrystal.h> //LCD Lib
#include <SR04.h>          // Ultrasonic Sensor Lib

//Define Pins
#define CALIBRATE_PIN 22
#define RECORDING_PIN 23
#define LED_PIN 3
#define ECHO_PIN 4
#define TRIG_PIN 5
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);  //this is defining the pins for LCD display

/*         Pin Table
 * | Pin A | Pin B | Color  |
 * |   1   |  N/A  |  N\A   |    // Not connected to anything
 * |   2   |  N/A  |  N\A   |    // Not connected to anything
 * |   3   |  LED  |  Red   |    // LED
 * |   4   |  Echo | Yellow |    // Ultrasonic Sensor
 * |   5   |  Trig |  Blue  |    // Ultrasonic Sensor
 * |   6   |  N/A  |  N\A   |    // Not connected to anything
 * |   7   |  RS   | Green  |    // LCD Display
 * |   8   |   E   | Oarnge |    // LCD Display
 * |   9   |   D4  |  Blue  |    // LCD Display
 * |   10  |   D5  |  Blue  |    // LCD Display
 * |   11  |   D6  |  Blue  |    // LCD Display
 * |   12  |   D7  |  Blue  |    // LCD Display
 * |   --  |   --  |   --   |
 * |   22  |  CAL  | Oarnge |    // Calibrate Button
 * |   23  |  REC  | Yellow |    // Recording Button
 * 
 *    Pin A = Arduino
 *    Pin B = Breadboard/Component
 *    
 *    5V Power
 */

//Define Vars
float CalibrateValue = 0;             // this is a calibrated distance that resembles a wall behind the pendulum
float CalibrateValueLast;             // this the previous calibrated distance
int CalState = 0;                     // Calibration Button
int RecState = 0;                     // Record Button
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);  // Setup Ultrasonic
long CurrentDistance;                 // Value read from distance sensor every frame of the program
long CurrentDistanceLast = 0;         // The previous CurrentDistance
long Period;                          // In Milliseconds
long StartTime;                       // point in time that the pendulum is seen
bool Calibrated = false;              // Weather or not the divice is calibrated
bool Recording = false;               // Weather or not the device is recording
bool AcceptCalibrateValue = false;    // Determining weather or not CalibrateValue is valid
int Passes = -1;                      // Number of passes of the pendulum
int PeriodList[0];                    // Array for storing periods

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(CALIBRATE_PIN, INPUT);
  pinMode(RECORDING_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  CurrentDistance=sr04.Distance(); 
  if (Calibrated && Recording) {
    lcd.setCursor(10, 1); lcd.print(Period);
    if ((CurrentDistance <= (CalibrateValue*0.75)) and (CurrentDistance != 0)) {    //if the pendulum is seen
      digitalWrite(LED_PIN, HIGH);
      if (Period > 0) {
        PeriodList[Passes] = Period;      //store the period
        Passes = Passes + 1;
      }
      Period = 0;
      StartTime = millis();
    } else {                                                                        //if pendulum is not seen
      digitalWrite(LED_PIN, LOW);
      Period = millis() - StartTime;
    }
  }
  
  //CALIBRATION
  CalState = digitalRead(CALIBRATE_PIN);
  if (CalState == LOW) {
    calibrate();
  }
  
  //RECORDING
  RecState = digitalRead(RECORDING_PIN);
  //Serial.println(digitalRead(RECORDING_PIN));
  if (RecState == LOW) {
    if (Recording) {
      recordingOff();
    } else {
      recordingOn();
    }
  }

  updateLCD();  //done every frame
}

void calibrate() {
  if (!Recording) {
    lcd.clear();
    CalibrateValueLast = CalibrateValue;
    CalibrateValue = sr04.Distance();
    AcceptCalibrateValue = TestAcceptC(CalibrateValue, 0, 30);
    if (AcceptCalibrateValue) {
      Calibrated = true;
    } else {
      lcd.clear();
      if (CalibrateValue == 0) {
        lcd.setCursor(0, 0); lcd.print("CAL Can't = 0");
      } else {
        lcd.setCursor(0, 0); lcd.print("CAL Can't be >30");
      }
      lcd.setCursor(0, 1); lcd.print("Try Again");
      delay(2000);
      lcd.clear();
      CalibrateValue = CalibrateValueLast;
    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Cannot Calibrate");
    lcd.setCursor(0, 1); lcd.print("Recording");
    delay(1000);
    lcd.clear();
  }
}

void updateLCD() {
  lcd.setCursor(0, 0);
  if (CurrentDistance != CurrentDistanceLast) {
    lcd.clear();
    lcd.print(CurrentDistance);
    CurrentDistanceLast = CurrentDistance;
  }
  
  lcd.setCursor(0, 1); lcd.print("CAL:");
  
  lcd.setCursor(4, 1); lcd.print(CalibrateValue);
  
  lcd.setCursor(14, 0); lcd.print("cm");

  /* //This is for testing purposes
  lcd.setCursor(12, 0); lcd.print(Recording);

  lcd.setCursor(10, 0); lcd.print(Calibrated);

  lcd.setCursor(8, 0); lcd.print(AcceptCalibrateValue);
  */
}

void recordingOff() {
  //display all the recorded values
  //wait for a responce
  //clear all the periods
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Recording: OFF");
  delay(1000);
  lcd.clear();
  Recording = false;
}

void recordingOn() {
  Period = 0;
  if (Calibrated) {
    AcceptCalibrateValue = TestAcceptC(CalibrateValue, 0, 30);
    if (AcceptCalibrateValue) {
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("Recording: ON");
      delay(1000);
      lcd.clear();
      Recording = true;
    }
  } else {
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("Not Calibrated");
      delay(2000);
      lcd.clear();
    }
}

bool TestAcceptC(float CalibrateValue, float MinC, float MaxC) {
  return CalibrateValue > MinC && CalibrateValue < MaxC;
}



