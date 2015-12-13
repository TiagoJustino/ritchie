#ifndef ROCKY_H
#define ROCKY_H
#include "pitches.h"

int rockyMelodyLength = 35;

int rockyMelody[] = { NOTE_B3,NOTE_D4,0,NOTE_E4,  NOTE_E4,NOTE_FS4,0,NOTE_B3,
  NOTE_B3,NOTE_D4,0,NOTE_E4,  NOTE_E4,NOTE_FS4,0,NOTE_B3,0,
  NOTE_A3,NOTE_G3,NOTE_A3,NOTE_G3,NOTE_A3,NOTE_B3,0,
  NOTE_G3,NOTE_G3,NOTE_G3,NOTE_G3,NOTE_G3,NOTE_G3,NOTE_G3,NOTE_G3,NOTE_G3,NOTE_C4,
  NOTE_B3 };

float rockyNoteDurations[] = {
  16,12,12,1.5,  16,12,12,1.5,  16,12,12,1.5,  16,12,12,1,12,
  16,16,12,16,16,3,12,  16,16,8,16,16,8,16,16,8,8, 
};

#endif
