#include "MMA8452Q.h"
#include <math.h>
#include "pitches.h"
#include "eyeofthetiger.h"
#include "nyancat.h"
#include "rocky.h"
#include "beep.h"
#include "unknownsong.h"


// at 25ms rate, 80 positions store 2s.
#define ACCEL_QUEUE_SIZE 20

#define SHAKE_TRESHOLD 1500000

#define LOW_SERVO_FREQUENCY 250
#define MEDIUM_SERVO_FREQUENCY 500
#define HIGH_SERVO_FREQUENCY 1000

#define INACTIVE 0
#define   ACTIVE 1

Servo servo;
Servo motor;

MMA8452Q accel;

unsigned long ledFrequency = 1000;
int buzzerPin = D2;
int motorPin = D3;
int ledPin = D4;
int servoPin = D5;
int debugLedPin = D7;
int servoPos = 180;
int debugLedState = HIGH;

// how long the robot will be active per cicle.
// for example, activeInterval = 10000 and cicleInterval = 900000 means:
// the robot will be active for 10s every 15 min.
unsigned long activeInterval;
unsigned long cicleInterval;
unsigned long lastActive;
unsigned long setIdleModeTime;

int servoFrequency;
boolean ledBlinking;
int melodyLengh;
int* melody;
float* melodyDurations;
int motorCurrentSpeed = 90;
int motorSpeed;
unsigned long motorMovementDuration;
unsigned long motorFrequency;

boolean shaking = false;

unsigned long now;
unsigned long ledCounter;

int accelQueuePos = 0;

struct {
  int x = 0, y = 0, z = 0;
} accelQueue[ACCEL_QUEUE_SIZE];

unsigned long readAccelCounter;
unsigned long ledSignalingCounter;

int state = INACTIVE;

void setIdleMode();
void setRegularMode();
void setImportantMode();
void setHabitMode();
void setGymMode();
int setMode(String arg);

void setup() {
  accel.init();
  Serial.begin(9600);

  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(debugLedPin, OUTPUT);
  motor.attach(motorPin);
  servo.attach(servoPin);

  digitalWrite(debugLedPin, debugLedState);

  motor.write(motorCurrentSpeed);
  servo.write(servoPos);

  Particle.function("setmode", setMode);
  setIdleMode();

  Serial.println("finished setup");
}

void setIdleMode() {
  state = INACTIVE;
  activeInterval = 0;
  cicleInterval = 0;
  servoFrequency = 0;
  ledBlinking = false;
  melody = NULL;
  melodyLengh = 0;
  melodyDurations = NULL;
  motorSpeed = 0;
  motorMovementDuration = 0;
  motorFrequency = 0;
}

void setRegularMode() {
  state = ACTIVE;
  activeInterval = 10000;
  cicleInterval = 900000; // 15 minutes
  servoFrequency = LOW_SERVO_FREQUENCY;
  ledBlinking = true;
  melody = beepMelody;
  melodyLengh = beepMelodyLength;
  melodyDurations = beepNoteDurations;
  motorSpeed = 0;
  motorMovementDuration = 0;
  motorFrequency = 0;
}

void setImportantMode() {
  state = ACTIVE;
  activeInterval = 10000;
  cicleInterval = 900000; // 15 minutes
  servoFrequency = HIGH_SERVO_FREQUENCY;
  ledBlinking = true;
  melody = unknownSongMelody;
  melodyLengh = unknownSongMelodyLength;
  melodyDurations = unknownSongNoteDurations;
  motorSpeed = 90;
  motorMovementDuration = 250;
  motorFrequency = 1000;
}

void setHabitMode() {
  state = ACTIVE;
  activeInterval = 10000;
  cicleInterval = 900000; // 15 minutes
  servoFrequency = MEDIUM_SERVO_FREQUENCY;
  ledBlinking = true;
  melody = rockyMelody;
  melodyLengh = rockyMelodyLength;
  melodyDurations = rockyNoteDurations;
  motorSpeed = 45;
  motorMovementDuration = 500;
  motorFrequency = 2000;
}

void setGymMode() {
  state = ACTIVE;
  activeInterval = 10000;
  cicleInterval = 900000; // 15 minutes
  servoFrequency = MEDIUM_SERVO_FREQUENCY;
  ledBlinking = true;
  melody = eyeMelody;
  melodyLengh = eyeMelodyLength;
  melodyDurations = eyeNoteDurations;
  motorSpeed = 45;
  motorMovementDuration = 500;
  motorFrequency = 2000;
}

int setMode(String arg) {
  int pos, value;
  char mode[64], strValue[64];
  const char *argp;

  // TODO Check the case of string without white space
  argp = arg.c_str();
  pos = strchr(argp, ' ') - argp;
  strncpy(mode, argp, pos);
  mode[pos] = '\0';
  strcpy(strValue, argp + pos + 1);
  value = atoi(strValue);

  if(!strcmp(mode, "idle")) {
    setIdleMode();
  } else if (!strcmp(mode, "regular")) {
    setRegularMode();
    setIdleModeTime = now + value;
  } else if (!strcmp(mode, "important")) {
    setImportantMode();
    setIdleModeTime = now + value;
  } else if (!strcmp(mode, "habit")) {
    setHabitMode();
    setIdleModeTime = now + value;
  } else if (!strcmp(mode, "gym")) {
    setGymMode();
    setIdleModeTime = now + value;
  } else {
    // invalid mode received
    return -1;
  }

  return 0;
}

void readAccel() {
  //int tap;

  accel.read();
  queueAccelRead();
}

void queueAccelRead() {
  accelQueue[accelQueuePos].x = accel.x;
  accelQueue[accelQueuePos].y = accel.y;
  accelQueue[accelQueuePos].z = accel.z;
  accelQueuePos++;
  accelQueuePos = accelQueuePos == ACCEL_QUEUE_SIZE ? 0 : accelQueuePos;
}

void toggleLed() {
  debugLedState = (debugLedState == HIGH ? LOW : HIGH);
  digitalWrite(debugLedPin, debugLedState);

  servoPos = (servoPos ==  180 ?   0 :  180);
  servo.write(servoPos);
}

double getXMean() {
  double sum = 0.0;
  for(int i = 0; i < ACCEL_QUEUE_SIZE; i++) {
    sum += accelQueue[i].x;
  }
  return sum / ((double) ACCEL_QUEUE_SIZE);
}

double getYMean() {
  double sum = 0.0;
  for(int i = 0; i < ACCEL_QUEUE_SIZE; i++) {
    sum += accelQueue[i].y;
  }
  return sum / ((double) ACCEL_QUEUE_SIZE);
}

double getZMean() {
  double sum = 0.0;
  for(int i = 0; i < ACCEL_QUEUE_SIZE; i++) {
    sum += accelQueue[i].z;
  }
  return sum / ((double) ACCEL_QUEUE_SIZE);
}

double getXVariance() {
  double sum = 0.0;
  double mean = getXMean();
  double dist;
  for(int i = 0; i < ACCEL_QUEUE_SIZE; i++) {
    dist = mean - accelQueue[i].x;
    sum += dist * dist;
  }
  return sum / ((double) ACCEL_QUEUE_SIZE);
}

double getYVariance() {
  double sum = 0.0;
  double mean = getYMean();
  double dist;
  for(int i = 0; i < ACCEL_QUEUE_SIZE; i++) {
    dist = mean - accelQueue[i].y;
    sum += dist * dist;
  }
  return sum / ((double) ACCEL_QUEUE_SIZE);
}

double getZVariance() {
  double sum = 0.0;
  double mean = getZMean();
  double dist;
  for(int i = 0; i < ACCEL_QUEUE_SIZE; i++) {
    dist = mean - accelQueue[i].z;
    sum += dist * dist;
  }
  return sum / ((double) ACCEL_QUEUE_SIZE);
}

void checkShake() {
  int xShake, yShake, zShake;
  char tmp[128];
  xShake = getXVariance() > SHAKE_TRESHOLD;
  yShake = getYVariance() > SHAKE_TRESHOLD;
  zShake = getZVariance() > SHAKE_TRESHOLD;
  sprintf(tmp, "%f %f %f", getXVariance(), getYVariance(), getZVariance());
  if(xShake || yShake || zShake) {
    if(!shaking) {
      Serial.println("Started Shaking.");
      shaking = true;
    }
  } else {
    if(shaking) {
      Serial.println("Stopped Shaking.");
      shaking = false;
    }
  }
}

void checkServo() {
  // TODO
}

void checkLed() {
  // TODO
}

void checkMelody() {
  // TODO
}

void checkMotor() {
  // TODO
}

void loop() {
  now = millis();

  if(now - readAccelCounter >= 25) {
    readAccelCounter = now;
    readAccel();
    checkShake();
  }

  if(now - ledCounter >= ledFrequency) {
    ledCounter = now;
    toggleLed();
    Serial.println("I'm alive");
  }

  if(now > setIdleModeTime) {
    setIdleMode();
  }

  if(cicleInterval && (now - lastActive > cicleInterval)) {
    state = ACTIVE;
    lastActive = now;
  }

  if(state == ACTIVE) {
    if(servoFrequency) {
      checkServo();
    }
    if(ledBlinking) {
      checkLed();
    }
    if(melody) {
      checkMelody();
    }
    if(motorSpeed) {
      checkMotor();
    }
    if(now - lastActive > activeInterval) {
      state = INACTIVE;
    }
  }
}
