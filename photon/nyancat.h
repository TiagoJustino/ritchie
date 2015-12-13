/*
 Nyan Cat
 
 Plays Nyan Cat
 example: http://www.youtube.com/watch?v=eFHfGA5e4n8
 circuit:
 * 8-ohm speaker on digital pin 8
  
 By: Bruce Helsen (geobruce) 
     Stan Draulans (purewantfun)
     
  */
#ifndef NYAN_CAT_H
#define NYAN_CAT_H
#include "pitches.h"

int nyanMelodyLength =241;

int nyanMelody[] = {
  // Intro
  NOTE_DS5,
  NOTE_E5,
  NOTE_FS5,
  NOTE_B5,
  NOTE_DS5,
  NOTE_E5,
  NOTE_FS5,
  NOTE_B5,
  NOTE_CS6,
  NOTE_DS6,
  NOTE_CS6,
  NOTE_AS5,
  NOTE_B5,
  NOTE_FS5,
  NOTE_DS5,
  NOTE_E5,
  NOTE_FS5,
  NOTE_B5,
  NOTE_CS6,
  NOTE_AS5,
  NOTE_B5,
  NOTE_CS6,
  NOTE_E6,
  NOTE_DS6,
  NOTE_E6,
  NOTE_B5, 
  //A
  NOTE_FS5,
  NOTE_GS5,
  NOTE_DS5,
  NOTE_DS5,
  REST,
  NOTE_B4,
  NOTE_D5,
  NOTE_CS5,
  NOTE_B4,
  REST,
  NOTE_B4,
  NOTE_CS5,
  NOTE_D5,
  NOTE_D5,
  NOTE_CS5,
  NOTE_B4,
  NOTE_CS5,
  NOTE_DS5,
  NOTE_FS5,
  NOTE_GS5,
  NOTE_DS5,
  NOTE_FS5,
  NOTE_CS5,
  NOTE_DS5,
  NOTE_B4,
  NOTE_CS5,
  NOTE_B4,
  NOTE_DS5,
  NOTE_FS5,
  NOTE_GS5,
  NOTE_DS5,
  NOTE_FS5,
  NOTE_CS5,
  NOTE_DS5,
  NOTE_B4,
  NOTE_D5,
  NOTE_DS5,
  NOTE_D5,
  NOTE_CS5,
  NOTE_B4,
  NOTE_CS5,
  NOTE_D5,
  NOTE_B4,
  NOTE_CS5,
  NOTE_DS5,
  NOTE_FS5,
  NOTE_CS5,
  NOTE_DS5,
  NOTE_CS5,
  NOTE_B4,
  NOTE_CS5,
  NOTE_B4,
  NOTE_CS5,
  NOTE_FS5,
  NOTE_GS5,
  NOTE_DS5,
  NOTE_DS5,
  REST,
  NOTE_B4,
  NOTE_D5, 
  NOTE_CS5,
  NOTE_B4,
  REST,
  NOTE_B4,
  NOTE_CS5,
  NOTE_D5,
  NOTE_D5,
  NOTE_CS5,
  NOTE_B4,
  NOTE_CS5,
  NOTE_DS5,
  NOTE_FS5,
  NOTE_GS5,
  NOTE_DS5,
  NOTE_FS5,
  NOTE_CS5,
  NOTE_DS5,
  NOTE_B4,
  NOTE_CS5,
  NOTE_B4,
  NOTE_DS5,
  NOTE_FS5,
  NOTE_GS5,
  NOTE_DS5,
  NOTE_FS5,
  NOTE_CS5,
  NOTE_DS5,
  NOTE_B4,
  NOTE_D5,
  NOTE_DS5,
  NOTE_D5,
  NOTE_CS5,
  NOTE_B4,
  NOTE_CS5,
  NOTE_D5,
  NOTE_B4,
  NOTE_CS5,
  NOTE_DS5,
  NOTE_FS5,
  NOTE_CS5,
  NOTE_DS5,
  NOTE_CS5,
  NOTE_B4,
  NOTE_CS5,
  NOTE_B4,
  NOTE_CS5,
  //end of loop
  NOTE_B4,
  NOTE_FS4,
  NOTE_GS4,
  NOTE_B4,
  NOTE_FS4,
  NOTE_GS4,
  NOTE_B4,
  NOTE_CS5,
  NOTE_DS5,
  NOTE_B4,
  NOTE_E5,
  NOTE_DS5,
  NOTE_E5,
  NOTE_FS5,
  NOTE_B4,
  NOTE_B4,
  NOTE_FS4,
  NOTE_GS4,
  NOTE_B4,
  NOTE_FS4,
  NOTE_E5,
  NOTE_DS5,
  NOTE_CS5,
  NOTE_B4,
  NOTE_FS4,
  NOTE_DS4,
  NOTE_E4,
  NOTE_FS4,
  NOTE_B4,
  NOTE_FS4,
  NOTE_GS4,
  NOTE_B4,
  NOTE_FS4,
  NOTE_GS4,
  NOTE_B4,
  NOTE_B4,
  NOTE_CS5,
  NOTE_DS5,
  NOTE_B4,
  NOTE_FS4,
  NOTE_GS4,
  NOTE_FS4,
  //
  NOTE_B4,
  NOTE_B4,
  NOTE_AS4,
  NOTE_B4,
  NOTE_FS4,
  NOTE_GS4,
  NOTE_E4,
  NOTE_E5,
  NOTE_DS5,
  NOTE_E5,
  NOTE_FS5,
  NOTE_B4,
  NOTE_AS4,
  //
  NOTE_B4,
  NOTE_FS4,
  NOTE_GS4,
  NOTE_B4,
  NOTE_FS4,
  NOTE_GS4,
  NOTE_B4,
  NOTE_CS5,
  NOTE_DS5,
  NOTE_B4,
  NOTE_E5,
  NOTE_DS5,
  NOTE_E5,
  NOTE_FS5,
  NOTE_B4,
  NOTE_B4,
  NOTE_FS4,
  NOTE_GS4,
  NOTE_B4,
  NOTE_FS4,
  NOTE_E5,
  NOTE_DS5,
  NOTE_CS5,
  NOTE_B4,
  NOTE_FS4,
  NOTE_DS4,
  NOTE_E4,
  NOTE_FS4,
  NOTE_B4,
  NOTE_FS4,
  NOTE_GS4,
  NOTE_B4,
  NOTE_FS4,
  NOTE_GS4,
  NOTE_B4,
  NOTE_B4,
  NOTE_CS5,
  NOTE_DS5,
  NOTE_B4,
  NOTE_FS4,
  NOTE_GS4,
  NOTE_FS4,
  NOTE_B4,
  NOTE_B4,
  NOTE_AS4,
  NOTE_B4,
  NOTE_FS4,
  NOTE_GS4,
  NOTE_B4,
  NOTE_E5,
  NOTE_DS5,
  NOTE_E5,
  NOTE_FS5,
  NOTE_B4,
  NOTE_CS5 
};

float nyanNoteDurations[] = {
  // Intro
  16,
  16,
  8,
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  8, //13
  8,
  16,
  16,
  8,
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  //25 intro
  //a
  8,
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  8,
  8,
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  8,
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16, 
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  8,
  8,
  8,
  8,
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  8,
  8,
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  8,
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16, 
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  8,
  8,
  8,
  //
  8,
  16,
  16,
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  8,
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  8,
  16,
  16,
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  //
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  8,
  8,
  //
  8,
  16,
  16,
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  8,
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  8,
  16,
  16,
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  //
  8,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  16,
  8,
  8 
};

#endif
