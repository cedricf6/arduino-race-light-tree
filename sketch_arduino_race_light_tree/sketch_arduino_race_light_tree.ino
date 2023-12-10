/*******************************************************************
* Copyright         : 2023 - Kenneth A & Cedric F
* File Name         : sketch_arduino_race_light_tree.ino
* Description       : This file contains the logic to control a custom made race light tree for RC cars.
* Version           : 0.17
/******************************************************************/

#include<Chrono.h>

#define R1_SENSOR 12               // Right Sensor No.1 placed in line with the Start Line (Line 1)
#define L1_SENSOR 11               // Left Sensor No.1 placed in line with the Start Line (Line 1) 
#define LEFT_PRE_STAGE_LIGHTS 10   // 2 LEFT PRE STAGE BULBS : old Pre1
#define RIGHT_PRE_STAGE_LIGHTS 9   // 2 RIGHT PRE STAGE BULBS : old Pre2
#define LEFT_STAGE_LIGHTS 8        // 2 LEFT STAGE LIGHTS : old Pre3
#define RIGHT_STAGE_LIGHTS 7       // 2 RIGHT STAGE LIGHTS : old Pre4
#define FIRST_YELLOW_LIGHTS 6      // FIRST PAIR OF YELLOW LIGHTS : old Yellow
#define SECOND_YELLOW_LIGHTS 5     // SECOND PAIR OF YELLOW LIGHTS : old Yellow1
#define THIRD_YELLOW_LIGHTS 4      // THIRD PAIR OF YELLOW LIGHTS : old Yellow2 
#define GREEN_LIGHTS 3             // PAIR OF GREEN LIGHTS : old Green

bool leftPreStage = false;
bool rightPreStage = false;
bool stageLight = false;
bool firstYellowLight = false;
bool secondYellowLight = false;
bool thirdYellowLight = false;
bool greenLight = false;
bool raceTreeLightStarted = false;
bool raceTreeLightEnded = false;
Chrono myChrono;

void setup()
{  
  // Stop the Chrono Timer since we will be using it in the loop function only
  myChrono.stop();

  // OUTPUT SETUP
  pinMode(LEFT_PRE_STAGE_LIGHTS, OUTPUT); 
  pinMode(RIGHT_PRE_STAGE_LIGHTS, OUTPUT);
  pinMode(LEFT_STAGE_LIGHTS, OUTPUT); 
  pinMode(RIGHT_STAGE_LIGHTS, OUTPUT);
  pinMode(FIRST_YELLOW_LIGHTS, OUTPUT);
  pinMode(SECOND_YELLOW_LIGHTS, OUTPUT); 
  pinMode(THIRD_YELLOW_LIGHTS, OUTPUT);
  pinMode(GREEN_LIGHTS, OUTPUT);

  // INPUT SETUP
  // Using INPUT_PULLUP for NPN Sensors and the logic is reversed 
  pinMode(L1_SENSOR, INPUT_PULLUP);
  pinMode(R1_SENSOR, INPUT_PULLUP);

  // Initalization Testing Stage
  turnOnAllTheLights();
  delay(1000);
  turnOffAllTheLights();
  delay(1000);
  turnOnAllTheLights();
  delay(1000);
  turnOffAllTheLights();
}

void loop()
{

  // If Left Sensor 1 IS blocked and Left Sensor 2 IS NOT blocked, turn ON the left PreStage Lights.
  // This means that the Left car is on the starting line but not overlapping the second left sensor.
  if (isL1Blocked() && !leftPreStage){
    turnOnPreStageLeft();
  // If both Left Sensors are unblocked in this situation, turn Left PreStage Lights OFF again.
  } else if (!isL1Blocked() && leftPreStage && !raceTreeLightStarted) {
    turnOffPreStageLeft();
    // If both Left Sensors are blocked in this situation, turn Left PreStage Lights OFF again.
  } else if (isL1Blocked() && leftPreStage && !raceTreeLightStarted) {
    turnOffPreStageLeft();
  }
  
  // If Right Sensor 1 IS blocked and Right Sensor 2 IS NOT blocked, turn ON the Right PreStage Lights.
  // This means that the Right car is on the starting line but not overlapping the second right sensor.
  if (isR1Blocked() && !rightPreStage){
    turnOnPreStageRight();
  // If both Right Sensors are unblocked in this situation, turn Right PreStage Lights OFF again.    
  } else if (!isR1Blocked() && rightPreStage && !raceTreeLightStarted) {
    turnOffPreStageRight();
  // If both Right Sensors are blocked in this situation, turn Right PreStage Lights OFF again.
  } else if (isR1Blocked() && rightPreStage && !raceTreeLightStarted) {
    turnOffPreStageRight();
  }

  // If Left Pre Stage AND Right Pre Stage are both OK we proceed with the lights 
  if (leftPreStage && rightPreStage && !raceTreeLightEnded){
    // Resume the Chrono Timer to use it instead of the delays
    myChrono.resume();
  
    // Wait 10 seconds so the car handlers can approach their positions.
    if (myChrono.hasPassed(10000, false) ) {
      continueRaceTreeLights();
    }
  }
}

bool isL1Blocked() {
  // LOW means Object Detected and hence return True for Left Sensor of Line #1.  
  return digitalRead(L1_SENSOR) == LOW ? false : true;
}

bool isR1Blocked() {
  // LOW means Object Detected and hence return True for Right Sensor of Line #1.
  return digitalRead(R1_SENSOR) == LOW ? false : true;
}


void turnOnPreStageLeft() {
  digitalWrite(LEFT_PRE_STAGE_LIGHTS, HIGH);
  leftPreStage = true;
}

void turnOnPreStageRight() {
  digitalWrite(RIGHT_PRE_STAGE_LIGHTS, HIGH);
  rightPreStage = true;
}

void turnOffPreStageLeft() {
  digitalWrite(LEFT_PRE_STAGE_LIGHTS, LOW);
  leftPreStage = false;
  myChrono.stop();
}

void turnOffPreStageRight() {
  digitalWrite(RIGHT_PRE_STAGE_LIGHTS, LOW);
  rightPreStage = false;
  myChrono.stop();
}

void turnOffAllTheLights() {
  // LOW MEANS TURN OFF THE LIGHTS
  digitalWrite(LEFT_PRE_STAGE_LIGHTS, LOW);
  digitalWrite(RIGHT_PRE_STAGE_LIGHTS, LOW);     
  digitalWrite(LEFT_STAGE_LIGHTS, LOW);
  digitalWrite(RIGHT_STAGE_LIGHTS, LOW);
  digitalWrite(FIRST_YELLOW_LIGHTS, LOW);     
  digitalWrite(SECOND_YELLOW_LIGHTS, LOW);     
  digitalWrite(THIRD_YELLOW_LIGHTS, LOW);    
  digitalWrite(GREEN_LIGHTS, LOW);
}

void turnOnAllTheLights() {
  // HIGH MEANS TURN ON THE LIGHTS
  digitalWrite(LEFT_PRE_STAGE_LIGHTS, HIGH);
  digitalWrite(RIGHT_PRE_STAGE_LIGHTS, HIGH);     
  digitalWrite(LEFT_STAGE_LIGHTS, HIGH);
  digitalWrite(RIGHT_STAGE_LIGHTS, HIGH);      
  digitalWrite(FIRST_YELLOW_LIGHTS, HIGH);     
  digitalWrite(SECOND_YELLOW_LIGHTS, HIGH);     
  digitalWrite(THIRD_YELLOW_LIGHTS, HIGH);    
  digitalWrite(GREEN_LIGHTS, HIGH);
}

void continueRaceTreeLights() {
  raceTreeLightStarted = true;

  // Turn on the pair of stage lights after two more seconds
  if (myChrono.hasPassed(12000) && stageLight==false) {
    digitalWrite(LEFT_STAGE_LIGHTS, HIGH);
    digitalWrite(RIGHT_STAGE_LIGHTS, HIGH);
    stageLight = true;
  }
  
  // Turn on the first pair of Yellow Lights after one more second
  if (myChrono.hasPassed(13000) && firstYellowLight==false) {
    digitalWrite(FIRST_YELLOW_LIGHTS, HIGH);
    firstYellowLight = true;
  }

  // Turn on the second pair of Yellow Lights after half a second more
  if (myChrono.hasPassed(13500) && secondYellowLight==false) {
    digitalWrite(SECOND_YELLOW_LIGHTS, HIGH);
    secondYellowLight = true;
  }

  // Turn on the third pair of Yellow Lights after half a second more
  if (myChrono.hasPassed(14000) && thirdYellowLight==false) {
    digitalWrite(THIRD_YELLOW_LIGHTS, HIGH);
    thirdYellowLight = true;
  }

  // Turn on the Green Lights after half a second more
  if (myChrono.hasPassed(14500) && greenLight==false) {
    digitalWrite(GREEN_LIGHTS, HIGH);
    greenLight = true;

    // Set the other Lights OFF Except for the False Lights
    digitalWrite(LEFT_PRE_STAGE_LIGHTS, LOW);
    digitalWrite(RIGHT_PRE_STAGE_LIGHTS, LOW);
    digitalWrite(LEFT_STAGE_LIGHTS, LOW);
    digitalWrite(RIGHT_STAGE_LIGHTS, LOW);
    digitalWrite(FIRST_YELLOW_LIGHTS, LOW);
    digitalWrite(SECOND_YELLOW_LIGHTS, LOW);
    digitalWrite(THIRD_YELLOW_LIGHTS, LOW);    
  }

  // Turn off the Green Lights after 10.5 seconds has passed and greenLight back to false.
  if (myChrono.hasPassed(25000) && greenLight==true) {
    digitalWrite(GREEN_LIGHTS, LOW);
    greenLight = false;
    raceTreeLightEnded = true; 
  }

}