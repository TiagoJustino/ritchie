#include "MMA8452Q.h"
#include <math.h>

// at 25ms rate, 80 positions store 2s.
#define ACCEL_QUEUE_SIZE 20

#define SHAKE_TRESHOLD 1500000

Servo servo;

MMA8452Q accel;

int ledFrequency = 1000;
int servoPin = D3;
int servoPos = 180;
int ledState = HIGH;

boolean shaking = false;

unsigned long now;
unsigned long ledCounter;

int accelQueuePos = 0;

struct {
  int x = 0, y = 0, z = 0;
} accelQueue[ACCEL_QUEUE_SIZE];

unsigned long readAccelCounter;
unsigned long ledSignalingCounter;

void setup() {
  accel.init();

  Serial.begin(9600);
  pinMode(D7, OUTPUT);
  digitalWrite(D7, ledState);
  servo.attach(servoPin);
  servo.write(servoPos);
  Serial.println("finished setup");
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
  }
}
