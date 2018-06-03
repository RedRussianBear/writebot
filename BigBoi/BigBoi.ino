#include <AccelStepper.h>
#include <MultiStepper.h>

const int STEP = 8;
const int MODE_HALF = 8;
const int MESSAGE_LEN = 1024;
const float STEPSPERREV = 2048;
const float MAX_SPEED = 800.0;
const float STEPSPERMM = 50.947;

const byte ABS = 1;
const byte REL = 2;

const int X = 0;
const int Y = 1;

boolean moving = false;

AccelStepper motorX(MODE_HALF, 2, 4, 3, 5);
AccelStepper motorY(MODE_HALF, 6, 8, 7, 9);
AccelStepper motorF(MODE_HALF, 10, 12, 11, 13);

MultiStepper motors;

long dests[2];

char message_store[MESSAGE_LEN + 1];

void moveXY(int dx, int dy);

void setup() {
  while (!Serial);
  Serial.begin(9600);

  motorX.setMaxSpeed(MAX_SPEED);
  motorY.setMaxSpeed(MAX_SPEED);

  motors.addStepper(motorX);
  motors.addStepper(motorY);

  Serial.println("Booted. ARD2.");
}


void loop() {

  if(Serial.available() > 0) {
    delay(50);
    int command_len = Serial.readBytes(message_store, MESSAGE_LEN);
    char* message = message_store;
    message[command_len] = 0;

    char type = *message;
    message++;

    float dX = 0;
    float dY = 0;
    byte mode = isupper(type) ? ABS : REL;
    
    switch(type) {
      case 's':
      case 'S':
        message = strtok(message, ",");
        while(message != NULL) {
          dX = STEPSPERMM * atof(message);
          message = strtok(NULL, ",");
          dY = -STEPSPERMM * atof(message);
          moveToXY(dX, dY, mode);  
          message = strtok(NULL, ",");
        }
        break;
      
      case 'm':
      case 'M':
        message = strtok(message, ",");
        dX = STEPSPERMM * atof(message);
        message = strtok(NULL, ",");
        dY = -STEPSPERMM * atof(message);
        moveToXY(dX, dY, mode);
        break;

      case 'x':
      case 'X':
        dX = STEPSPERMM * atof(message);
        moveToXY(dX, dY, mode);
        break;

      case 'y':
      case 'Y':
        dY = -STEPSPERMM * atof(message);
        moveToXY(dX, dY, mode);
        break;

      case 'c':
      case 'C':
        motorX.setCurrentPosition(0);
        motorY.setCurrentPosition(0);
        break;
    }

    
    Serial.println("Movement complete.");
  }

}


void moveToXY(float x, float y, byte mode) {
  float dx, dy;

  if(mode == REL) {
    dx = x + motorX.currentPosition();
    dy = y + motorY.currentPosition();
  }
  else if(mode == ABS) {
    dx = x;
    dy = y;
  }

  dests[0] = (long) dx;
  dests[1] = (long) dy;
  motors.moveTo(dests);
  motors.runSpeedToPosition();
  

}

