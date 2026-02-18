#ifndef MUSIC_H
#define MUSIC_H

#include <stdint.h>
#include <stdbool.h>
#include "LPC17xx.h"

/* --- NOTES FREQUENCIES (Hz) --- */
#define NOTE_G3  196
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_PAUSE 0

#define RHYTHM_TICK 2500000 

extern volatile uint16_t *currentTrack;
extern volatile int currentTrackLen;
extern volatile int noteIndex;
extern volatile bool isLooping;

void music_init(void);
void playNote(uint16_t freq);
void playEffect(int effectID);

#endif