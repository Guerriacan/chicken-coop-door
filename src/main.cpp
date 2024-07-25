#include <Arduino.h>
#include <OneButton.h>
#include <TimerOne.h>
#include <arduino-timer.h>
#include <singleLEDLibrary.h>

#define pinEnable A0
#define pinDirecA A1
#define pinDirecB A2
#define pinLightSensor A3
#define pinEndSwitchUp A4
#define pinRotaryEncoder A5
#define pinLedUp 3
#define pinButtonUp 2
#define pinLedCenterRed 5
#define pinLedCenterGreen 6
#define pinLedCenterBlue 9
#define pinbuttonMiddle 4
#define pinButtonDown 11
#define pinLedDown 10

auto timer = timer_create_default();

unsigned int lightSensorAverage;
unsigned int lightOpenLevel = 500;
unsigned int lightCloseLevel = 900;

sllib ledUp(pinLedUp);
sllib ledCenterRed(pinLedCenterRed);
sllib ledCenterGreen(pinLedCenterGreen);
sllib ledCenterBlue(pinLedCenterBlue);
sllib ledDown(pinLedDown);
void updateAllLeds();

enum HatchMode {AUTO, MANUAL, ERROR};
HatchMode currentHatchMode = AUTO;
bool hasModeChanged();
enum HatchState {OPEN, OPENING, CLOSED, CLOSING, STOP, UNKNOWN};
HatchState currentHatchState = STOP;
bool hasStateChanged();

int motorTurnCount;
byte motorTurnCountTarget = 45;
enum MotorControl {UP, DOWN, OFF};
void setMotor(MotorControl control);

OneButton buttonUp(pinButtonUp, true, true);
OneButton buttonMiddle(pinbuttonMiddle, true, true);
OneButton buttonDown(pinButtonDown, true, true);
OneButton rotaryEncoder(pinRotaryEncoder, true, true);
void buttonsTick();

unsigned int getLightSensorAverage();

void setup() {
  pinMode(pinEnable, OUTPUT);
  pinMode(pinDirecA, OUTPUT);
  pinMode(pinDirecB, OUTPUT);
  pinMode(pinLightSensor, INPUT);
  pinMode(pinEndSwitchUp, INPUT_PULLUP);
  digitalWrite(pinEnable, LOW);
  digitalWrite(pinDirecA, LOW);
  digitalWrite(pinDirecB, LOW);

  setMotor(OFF);
  Timer1.initialize(10000);
  Timer1.attachInterrupt(buttonsTick);
  buttonUp.attachClick([]() {
    currentHatchMode = MANUAL;
    if (currentHatchState != OPEN) {
      currentHatchState = OPENING;
    }
  });
  buttonMiddle.attachClick([]() {
    currentHatchMode = MANUAL;
    currentHatchState = STOP;
  });
  buttonMiddle.attachLongPressStart([]() {
    currentHatchMode = AUTO;
    currentHatchState = STOP;
  });
  buttonDown.attachClick([]() {
    currentHatchMode = MANUAL;
    if (currentHatchState != CLOSED) {
      currentHatchState = CLOSING;
    }
  });
  rotaryEncoder.attachClick([]() {
    if (currentHatchState == OPENING) {
      motorTurnCount--;
    } else if (currentHatchState == CLOSING) {
      motorTurnCount++;
    }
  });

  for (byte i = 0; i < 5; i++) {
    getLightSensorAverage();
  }
  timer.every(180000, [](void*) -> bool {
    lightSensorAverage = getLightSensorAverage();
    return true;
  });

  if (digitalRead(pinEndSwitchUp) != LOW) {
    setMotor(UP);
  }
  
  while (digitalRead(pinEndSwitchUp) != LOW) {
    ledCenterRed.breathSingle(1000);
    ledCenterGreen.breathSingle(1000);
  }
  ledCenterRed.setOffSingle();
  ledCenterGreen.setOffSingle();
  currentHatchState = STOP;
}

void loop() {
  delay(10);
  timer.tick();
  updateAllLeds();

  switch (currentHatchMode) {
  case MANUAL:
    hasModeChanged();
    ledCenterBlue.blinkSingle(100,2900);
    break;

  case AUTO:
    hasModeChanged();
    ledCenterGreen.blinkSingle(100,2900);
    break;
  
  case ERROR:
    if (hasModeChanged()) {
      setMotor(OFF);
    }
    ledCenterRed.blinkSingle(100,2900);
    return;
    break;
  
  default:
    break;
  }

  switch (currentHatchState) {
  case OPEN:
    if (hasStateChanged()) {
      motorTurnCount = 0;
      if (currentHatchMode == MANUAL) {
        ledUp.setOnSingle();
      }
    }
    if (currentHatchMode == MANUAL) {
      break;
    }
    if (lightSensorAverage > lightCloseLevel) {
      currentHatchState = CLOSING;
    }
    break;

  case OPENING:
    if (hasStateChanged()) {
      setMotor(UP);
      ledUp.setBreathSingle(1000);
    }
    if (digitalRead(pinEndSwitchUp) == LOW) {
      currentHatchState = STOP;
    }
    break;

  case CLOSED:
    if (hasStateChanged()) {
      if (currentHatchMode == MANUAL) {
        ledDown.setOnSingle();
      }
    }
    if (currentHatchMode == MANUAL) {
      break;
    }
    if (lightSensorAverage < lightOpenLevel) {
      currentHatchState = OPENING;
    }
    break;

  case CLOSING:
    if (hasStateChanged()) {
      setMotor(DOWN);
      ledDown.setBreathSingle(1000);
    }
    if (motorTurnCount >= motorTurnCountTarget) {
      currentHatchState = STOP;
    }
    break;

  case STOP:
    if (hasStateChanged()) {
      setMotor(OFF);
    }
    if (digitalRead(pinEndSwitchUp) == LOW) {
      currentHatchState = OPEN;
    } else if (motorTurnCount >= motorTurnCountTarget) {
      currentHatchState = CLOSED;
    } else {
      currentHatchState = UNKNOWN;
    }
    break;

  case UNKNOWN:
    if (hasStateChanged()) {
      ledUp.setBreathSingle(1000);
      ledDown.setBreathSingle(1000);
    }
    if (currentHatchMode == AUTO) {
      if (lightSensorAverage > lightCloseLevel) {
        currentHatchState = CLOSING;
      } else if (lightSensorAverage < lightOpenLevel) {
        currentHatchState = OPENING;
      } else {
        currentHatchState = OPENING;
      }
    }
    break;

  default:
    currentHatchMode = ERROR;
    break;
  }
}

void updateAllLeds() {
  ledUp.update();
  ledCenterRed.update();
  ledCenterGreen.update();
  ledCenterBlue.update();
  ledDown.update();
}

bool hasModeChanged() {
  static HatchMode previousHatchMode;
  if (currentHatchMode != previousHatchMode) {
    ledCenterRed.setOffSingle();
    ledCenterGreen.setOffSingle();
    ledCenterBlue.setOffSingle();
    previousHatchMode = currentHatchMode;
    return true;
  }
  return false;
}
bool hasStateChanged() {
  static HatchState previousHatchState;
  if (currentHatchState != previousHatchState) {
    ledUp.setOffSingle();
    ledDown.setOffSingle();
    previousHatchState = currentHatchState;
    return true;
  }
  return false;
}

void setMotor(MotorControl control) {
  switch (control) {
  case UP:
    digitalWrite(pinDirecA, HIGH);
    digitalWrite(pinDirecB, LOW);
    digitalWrite(pinEnable, HIGH);
    break;
  case DOWN:
    digitalWrite(pinDirecA, LOW);
    digitalWrite(pinDirecB, HIGH);
    digitalWrite(pinEnable, HIGH);
    break;
  case OFF:
    digitalWrite(pinDirecA, LOW);
    digitalWrite(pinDirecB, LOW);
    digitalWrite(pinEnable, LOW);
    break;
  }
}

void buttonsTick() {
  rotaryEncoder.tick();
  buttonUp.tick();
  buttonMiddle.tick();
  buttonDown.tick();
}

unsigned int getLightSensorAverage() {
  static unsigned int lightReadings[5];
  for (int i = 0; i < 4; i++) {
    lightReadings[i] = lightReadings[i + 1];
  }
  lightReadings[4] = analogRead(pinLightSensor);
  for (int i = 0; i < 5; i++) {
    lightSensorAverage += lightReadings[i];
  }
  lightSensorAverage = lightSensorAverage / 5;
  return lightSensorAverage;
}