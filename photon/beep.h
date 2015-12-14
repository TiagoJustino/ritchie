#ifndef BEEP_H
#define BEEP_H
#include "pitches.h"

int beepMelodyLength = 8;

int beepMelody[] = {
  NOTE_G7, REST, NOTE_G7, REST, NOTE_G7, REST, NOTE_G7, REST
};

float beepNoteDurations[] = {
  16,16,16,16,16,16,16,4
};

#endif
