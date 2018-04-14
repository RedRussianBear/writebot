#include <Servo.h>
#include <AccelStepper.h>

const int MESSAGE_LEN = 32;
const int STEPSPERREVOLUTION = 200;

const float BASE_SPEED = 200.0;
const float MMPERREVOLUTION = 8;
const float STEPSPERMM = STEPSPERREVOLUTION / MMPERREVOLUTION;

const byte ABS = 1;
const byte REL = 2;

const int stepPin = 2;
const int dirPin = 3;
const int servPin = 11;

AccelStepper motorZ(1, stepPin, dirPin);
Servo servo;

char message_store[MESSAGE_LEN + 1];
int angle = 0;

void moveZ(int dZ);

void setup() {
  
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  servo.attach(servPin);
  servo.write(angle);

  motorZ.setMaxSpeed(BASE_SPEED);
  motorZ.setAcceleration(100.0);

  while (!Serial);
  Serial.begin(9600);

  Serial.println("Booted");
}


void loop() {

  if(Serial.available() > 0) {
      int command_len = Serial.readBytes(message_store, MESSAGE_LEN);
      char* message = message_store;
      message[command_len] = 0;
  
      char type = *message;
      message += 1;

      int dZ = 0;
      int dP = 0;
      byte mode = isupper(type) ? ABS : REL;
      
      switch(type){
        case 'm':
        case 'M':
          message = strtok(message, ",");
          dZ = (int) (STEPSPERMM * atof(message));
          message = strtok(NULL, ",");
          dP = atoi(message);
          break;   

        case 'z':
        case 'Z':
          dZ = (int) (STEPSPERMM * atof(message));
          break;

        case 'p':
        case 'P':
          dP = atoi(message);
          break;

        case 'c':
        case 'C':
          motorZ.setCurrentPosition(0);
          break;
      }

      if(mode == ABS) {
        motorZ.moveTo(dZ);
        angle = dP;
      }
      else if(mode == REL){
        motorZ.move(dZ);
        angle += dP;
      }

      servo.write(angle);
      motorZ.runToPosition();
      Serial.println("Movement complete");
  }  
  
}

