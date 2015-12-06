#include "MMA8452Q.h"
#include <math.h>
#define ACCEL_QUEUE_SIZE 20

Servo servo;

MMA8452Q accel;

int ledFrequency = 1000;
int servoPin = D3;
int servoPos = 180;
int ledState = HIGH;

unsigned long now;
unsigned long ledCounter;

int accelQueuePos = 0;

struct {
  int x = 0, y = 0, z = 0;
} previousAccel, accelQueue[ACCEL_QUEUE_SIZE];

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

int getXAverage() {
  int sum = 0;
  for(int i = 0; i < ACCEL_QUEUE_SIZE; i++) {
    sum += accelQueue[i].x;
  }
  return sum / ACCEL_QUEUE_SIZE;
}

int getYAverage() {
  int sum = 0;
  for(int i = 0; i < ACCEL_QUEUE_SIZE; i++) {
    sum += accelQueue[i].y;
  }
  return sum / ACCEL_QUEUE_SIZE;
}

int getZAverage() {
  int sum = 0;
  for(int i = 0; i < ACCEL_QUEUE_SIZE; i++) {
    sum += accelQueue[i].z;
  }
  return sum / ACCEL_QUEUE_SIZE;
}

void loop() {
  now = millis();

  if(now - readAccelCounter >= 25) {
    readAccelCounter = now;
    readAccel();
    previousAccel.x = accel.x;
    previousAccel.y = accel.y;
    previousAccel.z = accel.z;
  }

  if(now - ledCounter >= ledFrequency) {
    ledCounter = now;
    toggleLed();
  }
}
