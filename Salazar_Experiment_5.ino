#include "SevSeg.h"
#include <Servo.h>


Servo servoEnt;
Servo servoExt;
SevSeg sevseg; //Initiate a seven segment controller object

int pos = 0;
int displayNum = 25;
bool entMax = false;
bool extMax = false;
const int trigPin = 10;
const int echoPin = 9;
const int buttonPin = 5;
const int buzzer = A5; 
int buttonState = 0;
long duration;
int distance;
const int trigLow = 2;
const int trigHigh  = 10;
const int servoPinEnt = 6;
const int servoPinExt = 3;
unsigned long currentMillis = 0;
unsigned long previousTrig_Millis = 0;
unsigned long previousStep_Millis = 0;
const int servoMinEntDegrees = 90; // the limits to servo movement
const int servoMaxEntDegrees = 180;
const int servoMinExtDegrees = 90; // the limits to servo movement
const int servoMaxExtDegrees = 180;
byte onTrigState = LOW;   
byte onEchoState = LOW;            
byte trigState = LOW;           
byte echoState = LOW;

char capacityNum[] = "2525";

int servoEntPosition = 90;     // the current angle of the servo - starting at 90.
int servoEntInterval = 1000; // initial millisecs between servo moves
int servoEntDelay = 1000;
int servoEntDegrees = 2; 

int servoExtPosition = 90; 
int servoExtInterval = 1000; // initial millisecs between servo moves
int servoExtDelay = 1000;   // the current angle of the servo - starting at 90.
int servoExtDegrees = 2; 

unsigned long previousEntServoMillis = 0; // the time when the servo was last moved
unsigned long previousExtServoMillis = 0; // the time when the servo was last moved


void setup() {
    pinMode(trigPin, OUTPUT); 
    pinMode(echoPin, INPUT); 
    pinMode(buttonPin, INPUT);
    pinMode(buzzer, OUTPUT);
    servoEnt.attach(servoPinEnt);
    servoExt.attach(servoPinExt);
    byte numDigits = 4;  
    byte digitPins[] = {2, A2, 4, A1};
    byte segmentPins[] = {A0, 7, 8, A3, A4, 11, 12, 13};
    bool resistorsOnSegments = 0; 
    // variable above indicates that 4 resistors were placed on the digit pins.
    // set variable to 1 if you want to use 8 resistors on the segment pins.
    sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, resistorsOnSegments);
    sevseg.setBrightness(90);
    Serial.begin(9600);
    servoEnt.write(servoEntPosition); // sets the initial position
    servoEnt.attach(servoPinEnt);
    servoExt.write(servoExtPosition); // sets the initial position
    servoExt.attach(servoPinExt);
}
void loop() {
    currentMillis = micros();
    sonarDistance(); 
    servoEntSweep();
    servoExtSweep();
    displayScreen();
    sevseg.setChars(capacityNum);
    sevseg.refreshDisplay(); // Must run repeatedly  
}

void sonarDistance() {
 currentMillis = micros();
  if (onTrigState == LOW) {
    if (currentMillis - previousTrig_Millis >= trigLow) {
      onTrigState = HIGH;
      digitalWrite(trigPin, HIGH);
      previousTrig_Millis += trigLow;
    }
  }
  else {
    if (currentMillis - previousTrig_Millis >= trigHigh) {
      onTrigState = LOW;
      digitalWrite(trigPin, LOW);
      previousTrig_Millis += trigHigh;
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      // Calculating the distance
      distance =  duration * 0.034 / 2;
  }
  }
 

}

void servoEntSweep() {
  if (distance < 10 && displayNum > 0) {
    if (currentMillis - previousEntServoMillis >= servoEntInterval && entMax == false) {
        // its time for another move
        previousEntServoMillis += servoEntInterval;
        servoEntPosition = servoMaxEntDegrees; // servoDegrees might be negative
        if (servoEntPosition >= servoMaxEntDegrees) {
          entMax = true;
          servoEntDegrees = - servoEntDegrees;
        }
    }

    else if (currentMillis - previousEntServoMillis >= servoEntInterval && entMax == true)   {
          // if the servo is at either extreme change the sign of the degrees to make it move the other way
      delay(5000);
      servoEntPosition = servoMinEntDegrees; 
      if (servoEntPosition <= servoMinEntDegrees) {
        entMax = false;
        servoEntDegrees = - servoEntDegrees;
        displayNum--;
      }
      
    }
  }
  else if (distance < 10 && displayNum <= 0){
    tone(buzzer, 1000); 
    delay(1000); 
    noTone(buzzer);  
    displayNum = 0;   
  }
  // make the servo move to the next position
  servoEnt.write(servoEntPosition);
  // and record the time when the move happened  
}

void servoExtSweep() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH && displayNum <= 24) {
    if (currentMillis - previousExtServoMillis >= servoExtInterval && extMax == false) {
        // its time for another move
        previousExtServoMillis += servoExtInterval;
        servoExtPosition = servoMaxExtDegrees; // servoDegrees might be negative
        if (servoExtPosition >= servoMaxExtDegrees) {
          extMax = true;
          servoExtDegrees = - servoExtDegrees;
        }
    }

    else if (currentMillis - previousExtServoMillis >= servoExtInterval && extMax == true)   {
          // if the servo is at either extreme change the sign of the degrees to make it move the other way
      delay(5000);
      servoExtPosition = servoMinExtDegrees; 
      if (servoExtPosition <= servoMinExtDegrees) {
        extMax = false;
        servoExtDegrees = - servoExtDegrees;
        displayNum++;
      }
      
    }
  }
  else if (buttonState == HIGH && displayNum >= 25){
    tone(buzzer, 500); 
    delay(1000); 
    noTone(buzzer);  
    displayNum = 25;   
  }
  // make the servo move to the next position
  servoExt.write(servoExtPosition);
  // and record the time when the move happened 
}

void displayScreen() {
  if (displayNum == 0) {
    capacityNum[0] = '0';
    capacityNum[1] = '0';
    capacityNum[2] = '0';
    capacityNum[3] = '0';
  }
  else if (displayNum == 1) {
    capacityNum[0] = '0';
    capacityNum[1] = '1';
    capacityNum[2] = '0';
    capacityNum[3] = '1';
  }
  else if (displayNum == 2) {
    capacityNum[0] = '0';
    capacityNum[1] = '2';
    capacityNum[2] = '0';
    capacityNum[3] = '2';
  }
  else if (displayNum == 3) {
    capacityNum[0] = '0';
    capacityNum[1] = '3';
    capacityNum[2] = '0';
    capacityNum[3] = '3';
  }
  else if (displayNum == 4) {
    capacityNum[0] = '0';
    capacityNum[1] = '4';
    capacityNum[2] = '0';
    capacityNum[3] = '4';
  }
  else if (displayNum == 5) {
    capacityNum[0] = '0';
    capacityNum[1] = '5';
    capacityNum[2] = '0';
    capacityNum[3] = '5';
  }
  else if (displayNum == 6) {
    capacityNum[0] = '0';
    capacityNum[1] = '6';
    capacityNum[2] = '0';
    capacityNum[3] = '6';
  }
  else if (displayNum == 7) {
    capacityNum[0] = '0';
    capacityNum[1] = '7';
    capacityNum[2] = '0';
    capacityNum[3] = '7';
  }
  else if (displayNum == 8) {
    capacityNum[0] = '0';
    capacityNum[1] = '8';
    capacityNum[2] = '0';
    capacityNum[3] = '8';
  }
  else if (displayNum == 9) {
    capacityNum[0] = '0';
    capacityNum[1] = '9';
    capacityNum[2] = '0';
    capacityNum[3] = '9';
  }
  else if (displayNum == 10) {
    capacityNum[0] = '1';
    capacityNum[1] = '0';
    capacityNum[2] = '1';
    capacityNum[3] = '0';
  }
  else if (displayNum == 11) {
    capacityNum[0] = '1';
    capacityNum[1] = '1';
    capacityNum[2] = '1';
    capacityNum[3] = '1';
  }
  else if (displayNum == 12) {
    capacityNum[0] = '1';
    capacityNum[1] = '2';
    capacityNum[2] = '1';
    capacityNum[3] = '2';
  }
  else if (displayNum == 13) {
    capacityNum[0] = '1';
    capacityNum[1] = '3';
    capacityNum[2] = '1';
    capacityNum[3] = '3';
  }
  else if (displayNum == 14) {
    capacityNum[0] = '1';
    capacityNum[1] = '4';
    capacityNum[2] = '1';
    capacityNum[3] = '4';
  }
  else if (displayNum == 15) {
    capacityNum[0] = '1';
    capacityNum[1] = '5';
    capacityNum[2] = '1';
    capacityNum[3] = '5';
  }
  else if (displayNum == 16) {
    capacityNum[0] = '1';
    capacityNum[1] = '6';
    capacityNum[2] = '1';
    capacityNum[3] = '6';
  }
  else if (displayNum == 17) {
    capacityNum[0] = '1';
    capacityNum[1] = '7';
    capacityNum[2] = '1';
    capacityNum[3] = '7';
  }
  else if (displayNum == 18) {
    capacityNum[0] = '1';
    capacityNum[1] = '8';
    capacityNum[2] = '1';
    capacityNum[3] = '8';
  }
  else if (displayNum == 19) {
    capacityNum[0] = '1';
    capacityNum[1] = '9';
    capacityNum[2] = '1';
    capacityNum[3] = '9';
  }
  else if (displayNum == 20) {
    capacityNum[0] = '2';
    capacityNum[1] = '0';
    capacityNum[2] = '2';
    capacityNum[3] = '0';
  }
  else if (displayNum == 21) {
    capacityNum[0] = '2';
    capacityNum[1] = '1';
    capacityNum[2] = '2';
    capacityNum[3] = '1';
  }
  else if (displayNum == 22) {
    capacityNum[0] = '2';
    capacityNum[1] = '2';
    capacityNum[2] = '2';
    capacityNum[3] = '2';
  }
  else if (displayNum == 23) {
    capacityNum[0] = '2';
    capacityNum[1] = '3';
    capacityNum[2] = '2';
    capacityNum[3] = '3';
  }
  else if (displayNum == 24) {
    capacityNum[0] = '2';
    capacityNum[1] = '4';
    capacityNum[2] = '2';
    capacityNum[3] = '4';
  }
  else if (displayNum == 25) {
    capacityNum[0] = '2';
    capacityNum[1] = '5';
    capacityNum[2] = '2';
    capacityNum[3] = '5';
  }

}
