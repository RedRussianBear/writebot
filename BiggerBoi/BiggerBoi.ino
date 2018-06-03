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
const int servPin = 5;
const int enablePin = 4;

AccelStepper motorZ(1, stepPin, dirPin);
AccelStepper motorF(8, 6, 8, 7, 9);
Servo servo;

char message_store[MESSAGE_LEN + 1];
int angle = 60;

void moveZ(int dZ);

void setup() {
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, HIGH);
  servo.attach(servPin);
  servo.write(angle);

  motorZ.setMaxSpeed(BASE_SPEED);
  motorZ.setAcceleration(100.0);

  motorF.setMaxSpeed(BASE_SPEED);
  motorF.setAcceleration(100.0);

  while (!Serial);
  Serial.begin(9600);

  digitalWrite(enablePin, LOW);
  Serial.println("Booted. ARD1.");
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
      int dF = 0;
      byte mode = isupper(type) ? ABS : REL;
      
      switch(type){
        case 'm':
        case 'M':
          message = strtok(message, ",");
          dZ = (int) (STEPSPERMM * atof(message));
          message = strtok(NULL, ",");
          dP = atoi(message);
          if(mode == ABS) {
            motorZ.moveTo(dZ);
            angle = dP;
          }
          else if(mode == REL){
            motorZ.move(dZ);
            angle += dP;
          }
          servo.write(angle);
          break;   

        case 'z':
        case 'Z':
          dZ = (int) (STEPSPERMM * atof(message));
          if(mode == ABS)
            motorZ.moveTo(dZ);
          else if(mode == REL)
            motorZ.move(dZ);
          break;

        case 'p':
        case 'P':
          dP = atoi(message);
          if(mode == ABS)
            angle = dP;
          else if(mode == REL)
            angle += dP;
          servo.write(angle);
          break;

        case 'f':
        case 'F':
          dF = (int) (STEPSPERMM * atof(message));
          if(mode == ABS)
            motorF.moveTo(dF);
          else if(mode == REL)
            motorF.move(dF);
          break;

        case 'c':
        case 'C':
          motorZ.setCurrentPosition(0);
          break;
      }

      motorZ.runToPosition();
      motorF.runToPosition();
      Serial.println("Movement complete");
  }  
  
}

