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

Servo servo;
Servo motor;

MMA8452Q accel;

int ledFrequency = 1000;
int buzzerPin = D2;
int motorPin = D3;
int ledPin = D4;
int servoPin = D5;
int servoPos = 180;
int ledState = HIGH;

int servoFrequency;
boolean LEDBlinking;
int melodyLengh;
int* melody;
float* melodyDurations;
unsigned long servoInterval;
unsigned LEDInterval;
int motorInterval;
int motorSpeed;
int motorMovementDuration;
int motorFrequency;

boolean shaking = false;

unsigned long now;
unsigned long ledCounter;

int accelQueuePos = 0;

struct {
  int x = 0, y = 0, z = 0;
} accelQueue[ACCEL_QUEUE_SIZE];

unsigned long readAccelCounter;
unsigned long ledSignalingCounter;

int setState(String arg) {
  int pos, value;
  char state[64], strValue[64];
  const char *argp;

  argp = arg.c_str();
  pos = strchr(argp, ' ') - argp;
  strncpy(state, argp, pos);
  state[pos] = '\0';
  strcpy(strValue, argp + pos + 1);
  value = atoi(strValue);

  if(!strcmp(state, "idle")) {
    Serial.println("Received idle");
  } else if (!strcmp(state, "regular")) {
    servoFrequency = LOW_SERVO_FREQUENCY;
    LEDBlinking = true;
    melody = beepMelody;
    melodyLengh = beepMelodyLength;
    melodyDurations = beepNoteDurations;
    servoInterval = 900000; // 15 minutes
    LEDInterval   = 900000;
    motorInterval = 0;
    motorSpeed = 0;
    motorMovementDuration = 0;
    motorFrequency = 0;
  } else if (!strcmp(state, "important")) {
    servoFrequency = HIGH_SERVO_FREQUENCY;
    LEDBlinking = true;
    melody = unknownSongMelody;
    melodyLengh = unknownSongMelodyLength;
    melodyDurations = unknownSongNoteDurations;
    servoInterval = 900000; // 15 minutes
    LEDInterval   = 900000;
    motorInterval = 900000;
    motorSpeed = 90;
    motorMovementDuration = 250;
    motorFrequency = 1000;
  } else if (!strcmp(state, "habit")) {
    servoFrequency = MEDIUM_SERVO_FREQUENCY;
    LEDBlinking = true;
    melody = rockyMelody;
    melodyLengh = rockyMelodyLength;
    melodyDurations = rockyNoteDurations;
    servoInterval = 900000; // 15 minutes
    LEDInterval   = 900000;
    motorInterval = 900000;
    motorSpeed = 45;
    motorMovementDuration = 500;
    motorFrequency = 2000;
  } else if (!strcmp(state, "gym")) {
    servoFrequency = MEDIUM_SERVO_FREQUENCY;
    LEDBlinking = true;
    melody = eyeMelody;
    melodyLengh = eyeMelodyLength;
    melodyDurations = eyeNoteDurations;
    servoInterval = 900000; // 15 minutes
    LEDInterval   = 900000;
    motorInterval = 900000;
    motorSpeed = 45;
    motorMovementDuration = 500;
    motorFrequency = 2000;
  }

  return 0;
}

void setup() {
  accel.init();

  Serial.begin(9600);
  pinMode(D7, OUTPUT);
  digitalWrite(D7, ledState);
  servo.attach(servoPin);
  servo.write(servoPos);
  Serial.println("finished setup");

  Particle.function("setstate", setState);
}

void readAccel() {
  int tap;

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
  ledState = (ledState == HIGH ? LOW : HIGH);
  digitalWrite(D7, ledState);

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
}
