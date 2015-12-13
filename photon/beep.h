#ifndef BEEP_H
#define BEEP_H
#include "pitches.h"

int beepMelodyLength = 9;

int beepMelody[] = {
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3, REST, NOTE_B3, NOTE_C4, REST
};

float beepNoteDurations[] = {
  8,16,16,8,8,8,8,8,8
};

#endif
