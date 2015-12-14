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

#define SHAKE_TRESHOLD 400000

#define LOW_SERVO_FREQUENCY 1500
#define MEDIUM_SERVO_FREQUENCY 1000
#define HIGH_SERVO_FREQUENCY 500

#define INACTIVE 0
#define   ACTIVE 1

#define SERVO_MAX_ANGLE 70
#define SERVO_MIN_ANGLE 40

#define FORWARD  1
#define BACKWARD 0

Servo servo;
Servo motor;

MMA8452Q accel;

unsigned long ledFrequency = 3000;
int buzzerPin = WKP;
int motorPin = D2;
int servoPin = D3;
int ledPin = D4;
int debugLedPin = D7;
int debugLedState = HIGH;
int ledPattern[] = {500, 500, 500, 1000};
int ledPatternIndex = 0;
int ledPatternLength = 4;

unsigned long servoNextCheck = 0;
int servoCurrentAngle = SERVO_MAX_ANGLE;
int servoTargetAngle = SERVO_MAX_ANGLE;
int servoDelay = 2;

// how long the robot will be active per cicle.
// for example, activeInterval = 10000 and cicleInterval = 900000 means:
// the robot will be active for 10s every 15 min.
unsigned long activeInterval;
unsigned long cicleInterval;
unsigned long lastActive;
unsigned long setIdleModeTime;
unsigned long nextMelodyNoteCounter;
unsigned long motorNextCheck;
unsigned long shakeStartTime;
unsigned long ledSignalingCounter;
unsigned long nyanShakeDuration = 3000;

boolean idle = true;
boolean motorMoving = false;
boolean nyan = false;
boolean sameNyanShake = false;
boolean dontDisturb = false;

int melodyNoteIndex;

int servoFrequency;
boolean ledBlinking;
int melodyLength;
int* melody;
float* melodyDurations;
int motorCurrentSpeed = 90;
int motorSpeed;
int motorDirection;
unsigned long motorMovementDuration;
unsigned long motorFrequency;

boolean shaking = false;

unsigned long now;
unsigned long ledNextCheck;

int accelQueuePos = 0;

struct {
  int x = 0, y = 0, z = 0;
} accelQueue[ACCEL_QUEUE_SIZE];

unsigned long readAccelCounter;
unsigned long debugLedCounter;

int state = INACTIVE;

const uint32_t VIBGYOR_Colors[] = {
  0xEE82EE, 0x4B0082, 0x0000FF, 0x00FF00, 0xFFFF00, 0xFFA500, 0xFF0000
};
const int VIBGYOR_Size = sizeof (VIBGYOR_Colors) / sizeof (uint32_t);
int VIBGYOR_Index;

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
  servo.write(servoCurrentAngle);

  Particle.function("setmode", setMode);
  setIdleMode();

  Serial.println("finished setup");
}

void setIdleMode() {
  Serial.println("going idle");
  digitalWrite(ledPin, LOW);
  servoCurrentAngle = SERVO_MAX_ANGLE;
  servo.write(servoCurrentAngle);
  idle = true;
  state = INACTIVE;
  activeInterval = 0;
  cicleInterval = 0;
  servoFrequency = 0;
  ledBlinking = false;
  melody = NULL;
  melodyLength = 0;
  melodyDurations = NULL;
  motorSpeed = 0;
  motorMovementDuration = 0;
  motorFrequency = 0;
}

void setRegularMode() {
  idle = false;
  state = ACTIVE;
  lastActive = now;
  motorNextCheck = now;
  activeInterval = 10000;
  cicleInterval = 900000; // 15 minutes
  servoFrequency = LOW_SERVO_FREQUENCY;
  ledBlinking = true;
  melody = beepMelody;
  melodyLength = beepMelodyLength;
  melodyDurations = beepNoteDurations;
  motorSpeed = 0;
  motorMovementDuration = 0;
  motorFrequency = 0;
}

void setImportantMode() {
  idle = false;
  state = ACTIVE;
  lastActive = now;
  motorNextCheck = now;
  activeInterval = 10000;
  cicleInterval = 900000; // 15 minutes
  servoFrequency = HIGH_SERVO_FREQUENCY;
  ledBlinking = true;
  melody = unknownSongMelody;
  melodyLength = unknownSongMelodyLength;
  melodyDurations = unknownSongNoteDurations;
  motorSpeed = 90;
  motorMovementDuration = 1000;
  motorFrequency = 1000;
}

void setHabitMode() {
  idle = false;
  state = ACTIVE;
  lastActive = now;
  motorNextCheck = now;
  activeInterval = 10000;
  cicleInterval = 900000; // 15 minutes
  servoFrequency = MEDIUM_SERVO_FREQUENCY;
  ledBlinking = true;
  melody = rockyMelody;
  melodyLength = rockyMelodyLength;
  melodyDurations = rockyNoteDurations;
  motorSpeed = 45;
  motorMovementDuration = 1500;
  motorFrequency = 2000;
}

void setGymMode() {
  idle = false;
  state = ACTIVE;
  lastActive = now;
  motorNextCheck = now;
  activeInterval = 10000;
  cicleInterval = 900000; // 15 minutes
  servoFrequency = MEDIUM_SERVO_FREQUENCY;
  ledBlinking = true;
  melody = eyeMelody;
  melodyLength = eyeMelodyLength;
  melodyDurations = eyeNoteDurations;
  motorSpeed = 45;
  motorMovementDuration = 1500;
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

void toggleDebugLed() {
  debugLedState = (debugLedState == HIGH ? LOW : HIGH);
  digitalWrite(debugLedPin, debugLedState);
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

void leaveNyanMode() {
  nyan = false;
  setIdleMode();
  LED_Signaling_Stop();
}

// copied from system/src/system_cloud_internal.cpp (photon firmware code)
void LED_Signaling_Override(void) {
  LED_SetSignalingColor(VIBGYOR_Colors[VIBGYOR_Index]);
  LED_On(LED_RGB);
  ++VIBGYOR_Index;
  if (VIBGYOR_Index >= VIBGYOR_Size) {
    VIBGYOR_Index = 0;
  }
}

void setNyanMode() {
  Serial.println("nyan mode active");

  nyan = true;
  idle = false;
  melodyNoteIndex = 0;
  melody = nyanMelody;
  melodyLength = nyanMelodyLength;
  melodyDurations = nyanNoteDurations;
  LED_Signaling_Start();
  ledSignalingCounter = now;
}

void checkShake() {
  int xShake, yShake, zShake;
  xShake = getXVariance() > SHAKE_TRESHOLD;
  yShake = getYVariance() > SHAKE_TRESHOLD;
  zShake = getZVariance() > SHAKE_TRESHOLD;
  if(xShake || yShake || zShake) {
    if(!shaking) {
      shakeStartTime = now;
      Serial.println("Started Shaking.");
      if(!idle) {
        tone(buzzerPin, NOTE_C7, 200);
        setIdleMode();
      }
      shaking = true;
    } else {
      if((now - shakeStartTime > nyanShakeDuration) && (!sameNyanShake)) {
        sameNyanShake = true;
        if(!nyan) {
          setNyanMode();
        } else {
          leaveNyanMode();
        }
      }
    }
  } else {
    if(shaking) {
      Serial.println("Stopped Shaking.");
      shaking = false;
      sameNyanShake = false;
    }
  }
}

void checkServo() {
  if(servoCurrentAngle != servoTargetAngle) {
    if(servoCurrentAngle < servoTargetAngle) {
      ++servoCurrentAngle;
    }
    if(servoCurrentAngle > servoTargetAngle) {
      --servoCurrentAngle;
    }
    servo.write(servoCurrentAngle);
    servoNextCheck = now + servoDelay;
  }
  if(servoCurrentAngle == servoTargetAngle) {
    if(servoTargetAngle == SERVO_MAX_ANGLE) {
      servoNextCheck = now + servoFrequency;
    }
    servoTargetAngle = servoTargetAngle == SERVO_MAX_ANGLE ? SERVO_MIN_ANGLE : SERVO_MAX_ANGLE;
  }
}

void checkLed() {
  ledNextCheck = now + ledPattern[ledPatternIndex];

  digitalWrite(ledPin, !(ledPatternIndex % 2));

  ++ledPatternIndex;
  if(ledPatternIndex == ledPatternLength) {
    ledPatternIndex = 0;
  }
}

void checkMelody() {
  if(now >= nextMelodyNoteCounter) {
    int thisNote = melody[melodyNoteIndex];
    int noteDuration = 880 / melodyDurations[melodyNoteIndex];
    int pauseBetweenNotes = noteDuration * 1.30;
    tone(buzzerPin, thisNote, noteDuration);
    nextMelodyNoteCounter += noteDuration + pauseBetweenNotes;
    melodyNoteIndex += 1;
    if(melodyNoteIndex == melodyLength) {
      melodyNoteIndex = 0;
    }
  }
}

void checkMotor() {
  int speed = 90;

  if(motorMoving) {
    motor.write(90);
    motorMoving = false;
    motorNextCheck = now + (motorFrequency - motorMovementDuration);
  } else {
    if(motorDirection == FORWARD) {
      speed = 90 - motorSpeed;
      motorDirection = BACKWARD;
    } else {
      speed = 90 + motorSpeed;
      motorDirection = FORWARD;
    }
    motor.write(speed);
    motorMoving = true;
    motorNextCheck = now + motorMovementDuration;
  }
}

void goInactive() {
  servoCurrentAngle = SERVO_MAX_ANGLE;
  servo.write(servoCurrentAngle);
  ledPatternIndex = 0;
  melodyNoteIndex = 0;
  motor.write(90);
  digitalWrite(ledPin, LOW);
  state = INACTIVE;
}

void checkNyan() {
  if(now - ledSignalingCounter >= 100) {
    ledSignalingCounter = now;
    LED_Signaling_Override();
  }
  checkMelody();
}

void checkOrientation() {
  byte orientation = accel.readPL(); 
  if(orientation != LOCKOUT) {
    if(!dontDisturb) {
      goInactive();
    }
    dontDisturb = true;
  } else {
    dontDisturb = false;
  }
}

void loop() {
  now = millis();

  if(now - readAccelCounter >= 25) {
    readAccelCounter = now;
    readAccel();
    checkShake();
    checkOrientation();
  }

  if(dontDisturb) {
    return;
  }

  if(now - debugLedCounter >= ledFrequency) {
    debugLedCounter = now;
    toggleDebugLed();
    Serial.println("I'm alive");
  }

  if(nyan) {
    checkNyan();
    return;
  }

  if(idle == false && now > setIdleModeTime) {
    setIdleMode();
  }

  if(cicleInterval && (now - lastActive > cicleInterval)) {
    Serial.println("going active");
    state = ACTIVE;
    lastActive = now;
  }

  if(accel.readTap() && state == ACTIVE) {
    tone(buzzerPin, NOTE_G7, 200);
    goInactive();
  }

  if(state == ACTIVE) {
    if(servoFrequency && (now >= servoNextCheck)) {
      checkServo();
    }
    if(ledBlinking && (now >= ledNextCheck)) {
      checkLed();
    }
    if(melody) {
      checkMelody();
    }
    if(motorSpeed && now >= motorNextCheck) {
      checkMotor();
    }
    if(now - lastActive > activeInterval) {
      goInactive();
    }
  }
}
