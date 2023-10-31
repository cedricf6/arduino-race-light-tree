/*******************************************************************
* Copyright         : 2023 - Kenneth A & Cedric F
* File Name         : sketch_arduino_race_light_tree.ino
* Description       : This file contains the logic to control a custom made race light tree for RC cars.
* Version           : 0.7
*                    
* Revision History  :
* Date				  Author      Comments
* ---------------------------------------------------------------------------
* 18/09/2023		Kenneth A		Created the first version using defalt sequence of instructions.
* 26/10/2023		Cedric F		Restrctured the program to use functions and added IR sensors functionality.
* 26/10/2023    Cedric F    Added functionality to control the PreStage Lights.
* 27/10/2023    Cedric F    Added functionaliy to check for false start.
* 30/10/2023    Cedric F    Fixed *_FALSE_START in the output setup
* 31/10/2023    Cedric F    Fixed the input pinmodes with INPUT_PULLUP and making sure that the PreStage lights are turned only once.
* 31/10/2023    Cedric F    Removed the delays in the loop function to avoid blockage and tweaked some more logics.
*
/******************************************************************/

#include<Chrono.h>

#define R2_SENSOR 0               // Right Sensor No.2 placed after Start Line (Line 2)
#define L2_SENSOR 13               // Left Sensor No.2 placed after Start line (Line 2)
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
#define L_FALSE_START 2            // Left Bottom Red Light to indicate left Car false start
#define R_FALSE_START 1            // Right Bottom Red Light to indicate right Car false start

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
  pinMode(L_FALSE_START, OUTPUT);
  pinMode(R_FALSE_START, OUTPUT);

  // INPUT SETUP
  // Using INPUT_PULLUP for NPN Sensors and the logic is reversed 
  // HIGH on the Pin means no detection
  pinMode(L1_SENSOR, INPUT_PULLUP);
  pinMode(R1_SENSOR, INPUT_PULLUP);
  pinMode(L2_SENSOR, INPUT_PULLUP);
  pinMode(R2_SENSOR, INPUT_PULLUP);

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
  // Checking for Left Car False start.
  // Second set of sensors (L2) on Line 2 for the Left Car to check for a false start.
  if (isL2Blocked() && greenLight==false && raceTreeLightStarted==true) {
    turnOnLeftFalseStartLight(); // This will keep on looping ON and OFF
  }

  // Checking for Right Car False start.
  // Second set of sensors (R2) on Line 2 for the Right Car to check for a false start.
  if (isR2Blocked() && greenLight==false && raceTreeLightStarted==true) {
    turnOnRightFalseStartLight(); // This will keep on looping ON and OFF
  }

  // If Left Sensor 1 IS blocked and Left Sensor 2 IS NOT blocked, turn on the left Pre Stage Lights.
  // This means that the Left car is on the starting line but not overlapping the second left sensor.
  if (isL1Blocked() && !isL2Blocked() && !leftPreStage){
    turnOnPreStageLeft();
  }
  
  // If Right Sensor 1 IS blocked and Right Sensor 2 IS NOT blocked, turn on the Right Pre Stage Lights.
  // This means that the Right car is on the starting line but not overlapping the second right sensor.
  if (isR1Blocked() && !isR2Blocked() && !rightPreStage){
    turnOnPreStageRight();
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
  return digitalRead(L1_SENSOR) == LOW;
}

bool isR1Blocked() {
  // LOW means Object Detected and hence return True for Right Sensor of Line #1.
  return digitalRead(R1_SENSOR) == LOW;
}

bool isL2Blocked() {
  // LOW means Object Detected and hence return True for Left Sensor of Line #2.
  return digitalRead(L2_SENSOR) == LOW;
}

bool isR2Blocked() {
  // LOW means Object Detected and hence return True for Right Sensor of Line #2.
  return digitalRead(R2_SENSOR) == LOW;
}

void turnOnPreStageLeft() {
  digitalWrite(LEFT_PRE_STAGE_LIGHTS, LOW);
  leftPreStage = true;
}

void turnOnPreStageRight() {
  digitalWrite(RIGHT_PRE_STAGE_LIGHTS, LOW);
  rightPreStage = true;
}

void turnOnLeftFalseStartLight() {
  turnOffAllTheLights();
  digitalWrite(L_FALSE_START, LOW); // Turn on the Left False Start Light
}

void turnOnRightFalseStartLight() {
  turnOffAllTheLights();
  digitalWrite(R_FALSE_START, LOW); // Turn on the Right False Start Light
}

void turnOffAllTheLights() {
  // HIGH MEANS TURN OFF THE LIGHTS - inverted cases
  digitalWrite(LEFT_PRE_STAGE_LIGHTS, HIGH);      
  digitalWrite(RIGHT_PRE_STAGE_LIGHTS, HIGH);     
  digitalWrite(LEFT_STAGE_LIGHTS, HIGH);    
  digitalWrite(RIGHT_STAGE_LIGHTS, HIGH);      
  digitalWrite(FIRST_YELLOW_LIGHTS, HIGH);     
  digitalWrite(SECOND_YELLOW_LIGHTS, HIGH);     
  digitalWrite(THIRD_YELLOW_LIGHTS, HIGH);    
  digitalWrite(GREEN_LIGHTS, HIGH);      
  digitalWrite(L_FALSE_START, HIGH);
  digitalWrite(R_FALSE_START, HIGH);
}

void turnOnAllTheLights() {
  // LOW MEANS TURN ON THE LIGHTS - inverted cases
  digitalWrite(LEFT_PRE_STAGE_LIGHTS, LOW);      
  digitalWrite(RIGHT_PRE_STAGE_LIGHTS, LOW);     
  digitalWrite(LEFT_STAGE_LIGHTS, LOW);
  digitalWrite(RIGHT_STAGE_LIGHTS, LOW);      
  digitalWrite(FIRST_YELLOW_LIGHTS, LOW);     
  digitalWrite(SECOND_YELLOW_LIGHTS, LOW);     
  digitalWrite(THIRD_YELLOW_LIGHTS, LOW);    
  digitalWrite(GREEN_LIGHTS, LOW);
  digitalWrite(L_FALSE_START, LOW);
  digitalWrite(R_FALSE_START, LOW);
}

void continueRaceTreeLights() {
  raceTreeLightStarted = true;

  // Turn on the pair of stage lights after two more seconds
  if (myChrono.hasPassed(12000) && stageLight==false) {
    digitalWrite(LEFT_STAGE_LIGHTS, LOW);
    digitalWrite(RIGHT_STAGE_LIGHTS, LOW);
    stageLight = true;
  }
  
  // Turn on the first pair of Yellow Lights after one more second
  if (myChrono.hasPassed(13000) && firstYellowLight==false) {
    digitalWrite(FIRST_YELLOW_LIGHTS, LOW); 
    firstYellowLight = true;
  }

  // Turn on the second pair of Yellow Lights after half a second more
  if (myChrono.hasPassed(13500) && secondYellowLight==false) {
    digitalWrite(SECOND_YELLOW_LIGHTS, LOW);
    secondYellowLight = true;
  }

  // Turn on the third pair of Yellow Lights after half a second more
  if (myChrono.hasPassed(14000) && thirdYellowLight==false) {
    digitalWrite(THIRD_YELLOW_LIGHTS, LOW);
    thirdYellowLight = true;
  }

  // Turn on the Green Lights after half a second more
  if (myChrono.hasPassed(14500) && greenLight==false) {
    digitalWrite(GREEN_LIGHTS, LOW);
    greenLight = true;

    // Set the other Lights OFF
    digitalWrite(LEFT_PRE_STAGE_LIGHTS, HIGH);   
    digitalWrite(RIGHT_PRE_STAGE_LIGHTS, HIGH);
    digitalWrite(LEFT_STAGE_LIGHTS, HIGH);
    digitalWrite(RIGHT_STAGE_LIGHTS, HIGH);
    digitalWrite(FIRST_YELLOW_LIGHTS, HIGH);
    digitalWrite(SECOND_YELLOW_LIGHTS, HIGH);
    digitalWrite(THIRD_YELLOW_LIGHTS, HIGH);
    digitalWrite(L_FALSE_START, HIGH);
    digitalWrite(R_FALSE_START, HIGH);
  }

  // Turn off the Green Lights after 5.5 seconds has passed and greenLight back to false.
  if (myChrono.hasPassed(20000) && greenLight==true) {
    digitalWrite(GREEN_LIGHTS, HIGH);
    greenLight = false;
    raceTreeLightEnded = true;
  }                

}