#include "music.h"
#include "../timer/timer.h"

/* --- ODE TO JOY --- */
static uint16_t melody_ode_to_joy[] = {
	/* --- PART A1 --- */
    NOTE_E4, NOTE_E4, NOTE_F4, NOTE_G4, 
    NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4, 
    NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4, 
    NOTE_E4, NOTE_PAUSE, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_PAUSE,
    
    /* --- PART A2 --- */
    NOTE_E4, NOTE_E4, NOTE_F4, NOTE_G4, 
    NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4, 
    NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4, 
    NOTE_D4, NOTE_PAUSE, NOTE_D4, NOTE_C4, NOTE_C4, NOTE_PAUSE,

    /* --- PART B --- */
    NOTE_D4, NOTE_D4, NOTE_E4, NOTE_C4,
    NOTE_D4, NOTE_E4, NOTE_F4, NOTE_E4, NOTE_C4,
    NOTE_D4, NOTE_E4, NOTE_F4, NOTE_E4, NOTE_D4,
    NOTE_C4, NOTE_D4, NOTE_G3, NOTE_PAUSE,

    /* --- PART A2 --- */
    NOTE_E4, NOTE_E4, NOTE_F4, NOTE_G4, 
    NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4, 
    NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4, 
    NOTE_D4, NOTE_PAUSE, NOTE_D4, NOTE_C4, NOTE_C4, NOTE_PAUSE,

    NOTE_PAUSE, NOTE_PAUSE
};

/* --- GAME OVER --- */
static uint16_t sfx_gameover[] = {
	NOTE_A4, NOTE_PAUSE,                 
	NOTE_A4, NOTE_PAUSE,                 
	NOTE_A4, NOTE_PAUSE,                
	NOTE_A4, NOTE_A4, NOTE_A4, NOTE_PAUSE,

	NOTE_C5, NOTE_PAUSE,                
	NOTE_B4, NOTE_PAUSE,                 
	NOTE_A4, NOTE_A4, NOTE_A4, NOTE_A4, NOTE_A4,
	
	NOTE_PAUSE, NOTE_PAUSE
};

volatile uint16_t *currentTrack;
volatile int currentTrackLen;
volatile int noteIndex;
volatile bool isLooping;

void music_init(void) {
	LPC_PINCON->PINSEL1 |= (1 << 21); // set P0.26 to DAC function
	LPC_PINCON->PINSEL1 &= ~(1 << 20);
	LPC_GPIO0->FIODIR |= (1<<26);
	
	
	init_timer(2, RHYTHM_TICK);
	enable_timer(2);
	
	init_timer(3, 0);
	
	currentTrack = melody_ode_to_joy;
	currentTrackLen = sizeof(melody_ode_to_joy)/sizeof(uint16_t);
	noteIndex = 0;
	isLooping = true;
}

void playNote(uint16_t freq) {
	if (freq == 0)
		LPC_TIM3->TCR = 0; // stop tone generator
	else {
		// PCLK / (freq * 2) for toggle
		LPC_TIM3->MR0 = 25000000 / (freq * 2);
		LPC_TIM3->TC = 0;
		LPC_TIM3->TCR = 1; // start/restart
	}
}

void playEffect(int effectID) {
	if (effectID == 0) {
		currentTrack = sfx_gameover;
		currentTrackLen = sizeof(sfx_gameover)/sizeof(uint16_t);
		noteIndex = 0;
		isLooping = false;
	}
}