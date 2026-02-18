#include "button.h"
#include "../game/tetris.h"
#include "../timer/timer.h"
#include "../GLCD/GLCD.h"

void EINT0_IRQHandler (void)	  	/* INT0														 */
{		
	LPC_SC->EXTINT |= (1 << 0);     /* clear pending interrupt         */
}

/* KEY1														 */
void EINT1_IRQHandler (void) {
	NVIC_DisableIRQ(EINT1_IRQn);

	/* --- GAME START / PAUSE LOGIC --- */
	if (currentState == STATE_GAMEOVER){
		NVIC_ClearPendingIRQ(TIMER0_IRQn);
		NVIC_ClearPendingIRQ(RIT_IRQn);
		NVIC_ClearPendingIRQ(EINT2_IRQn);
		LPC_SC->EXTINT |= (1 << 2);
		
		tetris_init();
	}
		
	if (currentState == STATE_IDLE) { // start the game
		music_init();
		currentState = STATE_PLAYING;
	
		srand(LPC_TIM0->TC); // allow spawning different shape types after reset
		spawnNewPiece();
		drawTetromino(currentX, currentY, currentRotation, currentPieceID, false);
	
		reset_timer(0);       
		enable_timer(0); // start tetronimos dropping down
		enable_RIT(); // start RIT to check joystick
	
		// clear pending KEY 2 interrupts
		NVIC_ClearPendingIRQ(EINT2_IRQn);
		LPC_SC->EXTINT |= (1 << 2);
	
		NVIC_EnableIRQ(EINT2_IRQn);
	}
	else if (currentState == STATE_PLAYING) { // pause the game and waiting for a new press of KEY 1
		currentState = STATE_PAUSED;
		
		GUI_Text(80, 140, (uint8_t *)"PAUSED", White, Black);
		disable_timer(0);
		disable_RIT();
		NVIC_DisableIRQ(EINT2_IRQn);
	}
	else if (currentState == STATE_PAUSED) { // resume the game
		currentState = STATE_PLAYING;
		
		LCD_FillRect(75, 140, 90, 24, Black); // erase "PAUSED" text
		refreshPlayingField(0, ROWS-1); // refresh the grid that overlaps with "PAUSED" text
		drawTetromino(currentX, currentY, currentRotation, currentPieceID, false); //redraw the spawned tetornimo
	
		enable_timer(0);
		enable_RIT();
		NVIC_EnableIRQ(EINT2_IRQn);
	}
	
	NVIC_EnableIRQ(EINT1_IRQn);
	LPC_SC->EXTINT |= (1 << 1);     /* clear pending interrupt         */
}

/* KEY2														 */
void EINT2_IRQHandler (void) {
	if (currentState == STATE_PLAYING) {
		disable_timer(0);
		disable_RIT();
		NVIC_DisableIRQ(EINT1_IRQn);
		NVIC_DisableIRQ(EINT2_IRQn);

		drawTetromino(currentX, currentY, currentRotation, currentPieceID, true); // erase current position
		for (; checkCollision(currentX, currentY + 1, currentRotation, currentPieceID) == false; currentY++); // keep moving down until tetronimo hits something
		drawTetromino(currentX, currentY, currentRotation, currentPieceID, false); // draw new position
		
		enable_timer(0);
		enable_RIT();
		NVIC_EnableIRQ(EINT1_IRQn);
  	}
	
  	LPC_SC->EXTINT |= (1 << 2);     /* clear pending interrupt         */    
}


