/*******************************************************************
* Copyright         : 2023 - Kenneth A & Cedric F
* File Name         : sketch_arduino_race_light_tree.ino
* Description       : This file contains the logic to control a custom made race light tree for RC cars. with CHATGPT optimizations
* Version           : 0.14
/******************************************************************/

#include <Chrono.h>

#define R2_SENSOR 0
#define L2_SENSOR 13
#define R1_SENSOR 12
#define L1_SENSOR 11
#define LEFT_PRE_STAGE_LIGHTS 10
#define RIGHT_PRE_STAGE_LIGHTS 9
#define LEFT_STAGE_LIGHTS 8
#define RIGHT_STAGE_LIGHTS 7
#define FIRST_YELLOW_LIGHTS 6
#define SECOND_YELLOW_LIGHTS 5
#define THIRD_YELLOW_LIGHTS 4
#define GREEN_LIGHTS 3
#define L_FALSE_START 2
#define R_FALSE_START 1

bool leftPreStage = false;
bool rightPreStage = false;
bool stageLight = false;
bool firstYellowLight = false;
bool secondYellowLight = false;
bool thirdYellowLight = false;
bool greenLight = false;
bool leftFalseLight = false;
bool rightFalseLight = false;
bool raceTreeLightStarted = false;
bool raceTreeLightEnded = false;
Chrono myChrono;

void setup() {
  myChrono.stop();
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

  pinMode(L1_SENSOR, INPUT_PULLUP);
  pinMode(R1_SENSOR, INPUT_PULLUP);
  pinMode(L2_SENSOR, INPUT_PULLUP);
  pinMode(R2_SENSOR, INPUT_PULLUP);

  initializeLights();
}

void loop() {
  checkFalseStarts();
  checkLeftPreStage();
  checkRightPreStage();

  if (leftPreStage && rightPreStage && !raceTreeLightEnded) {
    myChrono.resume();
    if (myChrono.hasPassed(10000, false)) {
      continueRaceTreeLights();
    }
  }
}

// Function to check for false starts
void checkFalseStarts() {
  if (isL2Blocked() && !greenLight && raceTreeLightStarted && !leftFalseLight && !raceTreeLightEnded) {
    turnOnLeftFalseStartLight();
  }

  if (isR2Blocked() && !greenLight && raceTreeLightStarted && !rightFalseLight && !raceTreeLightEnded) {
    turnOnRightFalseStartLight();
  }
}

// Function to check left pre-stage
void checkLeftPreStage() {
  if (isL1Blocked() && !isL2Blocked() && !leftPreStage) {
    turnOnPreStage(LEFT_PRE_STAGE_LIGHTS, leftPreStage);
  } else if (!isL1Blocked() && !isL2Blocked() && leftPreStage && !raceTreeLightStarted) {
    turnOffPreStage(LEFT_PRE_STAGE_LIGHTS);
  } else if (isL1Blocked() && isL2Blocked() && leftPreStage && !raceTreeLightStarted) {
    turnOffPreStage(LEFT_PRE_STAGE_LIGHTS);
  }
}

// Function to check right pre-stage
void checkRightPreStage() {
  if (isR1Blocked() && !isR2Blocked() && !rightPreStage) {
    turnOnPreStage(RIGHT_PRE_STAGE_LIGHTS, rightPreStage);
  } else if (!isR1Blocked() && !isR2Blocked() && rightPreStage && !raceTreeLightStarted) {
    turnOffPreStage(RIGHT_PRE_STAGE_LIGHTS);
  } else if (isR1Blocked() && isR2Blocked() && rightPreStage && !raceTreeLightStarted) {
    turnOffPreStage(RIGHT_PRE_STAGE_LIGHTS);
  }
}

// Function to continue the race tree lights
void continueRaceTreeLights() {
  raceTreeLightStarted = true;
  myChrono.resume();

  // Helper function to turn on various stage lights with a specified delay
  turnOnStageLights(LEFT_STAGE_LIGHTS, RIGHT_STAGE_LIGHTS, 12000, stageLight);
  turnOnStageLights(FIRST_YELLOW_LIGHTS, SECOND_YELLOW_LIGHTS, 1000, firstYellowLight);
  turnOnStageLights(SECOND_YELLOW_LIGHTS, THIRD_YELLOW_LIGHTS, 500, secondYellowLight);
  turnOnStageLights(THIRD_YELLOW_LIGHTS, GREEN_LIGHTS, 500, thirdYellowLight);

  if (myChrono.hasPassed(10500) && greenLight) {
    turnOffAllTheLightsExceptFalse();
    raceTreeLightEnded = true;
  }
}

void initializeLights() {
  turnOnAllTheLights();
  delay(1000);
  turnOffAllTheLights();
  delay(1000);
  turnOnAllTheLights();
  delay(1000);
  turnOffAllTheLights();
}

void turnOnPreStage(int preStageLight, bool &preStageFlag) {
  digitalWrite(preStageLight, HIGH);
  preStageFlag = true;
}

void turnOffPreStage(int preStageLight) {
  digitalWrite(preStageLight, LOW);
  myChrono.stop();
}

void turnOnLeftFalseStartLight() {
  digitalWrite(L_FALSE_START, HIGH);
  leftFalseLight = true;
}

void turnOnRightFalseStartLight() {
  digitalWrite(R_FALSE_START, HIGH);
  rightFalseLight = true;
}

void turnOnAllTheLights() {
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

void turnOffAllTheLights() {
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

void turnOnStageLights(int light1, int light2, int delayTime, bool &lightFlag) {
  if (myChrono.hasPassed(delayTime) && !lightFlag) {
    digitalWrite(light1, HIGH);
    digitalWrite(light2, HIGH);
    lightFlag = true;
  }
}

void turnOffAllTheLightsExceptFalse() {
  digitalWrite(LEFT_PRE_STAGE_LIGHTS, LOW);
  digitalWrite(RIGHT_PRE_STAGE_LIGHTS, LOW);
  digitalWrite(LEFT_STAGE_LIGHTS, LOW);
  digitalWrite(RIGHT_STAGE_LIGHTS, LOW);
  digitalWrite(FIRST_YELLOW_LIGHTS, LOW);
  digitalWrite(SECOND_YELLOW_LIGHTS, LOW);
  digitalWrite(THIRD_YELLOW_LIGHTS, LOW);
  digitalWrite(GREEN_LIGHTS, LOW);
}

bool isL1Blocked() {
  return digitalRead(L1_SENSOR) == LOW;
}

bool isR1Blocked() {
  return digitalRead(R1_SENSOR) == LOW;
}

bool isL2Blocked() {
  return digitalRead(L2_SENSOR) == LOW;
}

bool isR2Blocked() {
  return digitalRead(R2_SENSOR) == LOW;
}