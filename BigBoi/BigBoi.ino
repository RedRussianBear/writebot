#include <AccelStepper.h>

const int STEP = 8;
const int MODE_HALF = 8;
const int MESSAGE_LEN = 32;
const float STEPSPERREV = 2048;
const float BASE_SPEED = 160 * STEPSPERREV;
const float STEPSPERMM = 50.947;

const byte ABS = 1;
const byte REL = 2;

const int X = 0;
const int Y = 1;

boolean moving = false;

AccelStepper motorX(MODE_HALF, 2, 4, 3, 5);
AccelStepper motorY(MODE_HALF, 6, 8, 7, 9);

char message_store[MESSAGE_LEN + 1];

void moveXY(int dx, int dy);

void setup() {
  while (!Serial);
  Serial.begin(9600);

  motorX.setMaxSpeed(BASE_SPEED);
  motorX.setAcceleration(BASE_SPEED);
  motorX.setSpeed(BASE_SPEED);
  motorY.setMaxSpeed(BASE_SPEED);
  motorY.setAcceleration(BASE_SPEED);
  motorY.setSpeed(BASE_SPEED);

  Serial.println("Booted");
}


void loop() {

  if(Serial.available() > 0) {
    int command_len = Serial.readBytes(message_store, MESSAGE_LEN);
    char* message = message_store;
    message[command_len] = 0;

    char type = *message;
    message++;

    float dX = 0;
    float dY = 0;
    byte mode = isupper(type) ? ABS : REL;
    
    switch(type) {
      case 'm':
      case 'M':
        message = strtok(message, ",");
        dX = STEPSPERMM * atof(message);
        message = strtok(NULL, ",");
        dY = STEPSPERMM * atof(message);
        break;

      case 'x':
      case 'X':
        dX = STEPSPERMM * atof(message);
        break;

      case 'y':
      case 'Y':
        dY = STEPSPERMM * atof(message);
        break;

      case 'c':
      case 'C':
        motorX.setCurrentPosition(0);
        motorY.setCurrentPosition(0);
        break;
    }

    moveToXY(dX, dY, mode);
  }

  motorX.runSpeedToPosition();
  motorY.runSpeedToPosition();

  if(moving && !motorX.distanceToGo() && !motorY.distanceToGo()) {
    moving = false;
    Serial.println("Movement complete.");
  }
}


void moveToXY(float x, float y, byte mode) {
  float dx, dy;

  Serial.println(x);
  Serial.println(y);

  if(mode == ABS) {
    dx = dx - motorX.currentPosition();
    dy = dy - motorY.currentPosition();
  }
  else if(mode == REL) {
    dx = x;
    dy = y;
  }

  if(!dx && !dy) return;


  if(abs(dx) < abs(dy)) {
    motorX.setSpeed(abs(BASE_SPEED * dx / dy));
    motorY.setSpeed(BASE_SPEED);
  } else {
    motorX.setSpeed(BASE_SPEED);
    motorY.setSpeed(abs(BASE_SPEED * dy / dx));
  }

  if(mode == ABS) {
    motorX.moveTo((int) x);
    motorY.moveTo((int) y);
  }
  else if(mode == REL) {
    motorX.move((int) x);
    motorY.move((int) y);
  }
  moving = true;
}

