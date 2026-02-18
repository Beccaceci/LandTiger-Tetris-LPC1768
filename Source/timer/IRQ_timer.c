/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0, T1, T2 and T3 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "timer.h"
#include "../game/tetris.h"
#include "../game/graphics.h"
#include "../GLCD/GLCD.h"
#include "../adc/adc.h"
#include "../music/music.h"

#define J_DOWN (1 << 26)

#define MIN_INTERVAL 25000000 // 1.0s
#define MAX_INTERVAL 5000000  // 0.2s
#define INTERVAL_STEP 4884

/**
 * @brief Disables the joystick and button interrupts.
*/
static void disableInterrupts (void) {
	NVIC_DisableIRQ(RIT_IRQn);
	NVIC_DisableIRQ(EINT1_IRQn);
	NVIC_DisableIRQ(EINT2_IRQn);
}


/**
 * @brief Resets the RIT counter and re-enables all critical interrupts.
*/
static void enableInterrupts (void) {
	LPC_RIT->RICOUNTER = 0;
	NVIC_EnableIRQ(RIT_IRQn);
	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_EnableIRQ(EINT2_IRQn);
}


/**
 * @brief Handles the game over state by stopping the physics timer and joystick, clearing the screen,
 * displaying the "GAME OVER" message and ensures only KEY1 is enabled.
*/
static void gameOver (void) {
	disable_timer(0);
	disable_RIT();
	NVIC_DisableIRQ(EINT2_IRQn);
	currentState = STATE_GAMEOVER;
	
	playEffect(0);
	
  LCD_Clear(Black);
  GUI_Text(90, 140, (uint8_t *)"GAME OVER", Red, Black);
	
	NVIC_ClearPendingIRQ(EINT1_IRQn);
  NVIC_EnableIRQ(EINT1_IRQn);
}


/******************************************************************************
** Function name:   TIMER0_IRQHandler
**
** Descriptions:    Handles the physics tick.
** Fires every 1 second or faster.
******************************************************************************/
void TIMER0_IRQHandler (void) {
	uint32_t current_interval;
	if (slowMotionTimer > 0) {
		current_interval = MIN_INTERVAL;   
		slowMotionTimer--;
	}
	else{
		// interval = maxTime - (ADC_Value * Slope)
		current_interval = MIN_INTERVAL - (AD_current * INTERVAL_STEP);
		
		// start the next ADC conversion
		ADC_start_conversion();
	}
	
	// check for soft drop and doubles the current falling speed if the joystick is holding down
	if ((LPC_GPIO1->FIOPIN & J_DOWN) == 0)
		current_interval /= 2;
	
	// update the MR0 value with the new speed
	LPC_TIM0->MR0 = current_interval;
	
	if (currentState == STATE_PLAYING) {
		disableInterrupts();
		if (checkCollision(currentX, currentY + 1, currentRotation, currentPieceID) == true) { // check if (currentY + 1) hits anything
			lockPiece(); // lock the piece into the matrix
			
			int lines = removeFullLines();
		
			// calculate the obtained score
			score += 10;  // add 10 points for placement
			score += calculateScoreForLines(lines);
			
			if (score > highScore) // update high score
				highScore = score;
			
			updateScoreDisplay();
			
			// check game over condition
			if (currentY <= 0 || currentState == STATE_GAMEOVER) {
				gameOver();
				LPC_TIM0->IR = 1;
				return;
			}
			else {
				spawnNewPiece();
				
				if (checkCollision(currentX, currentY, currentRotation, currentPieceID) == true) {
					gameOver();
					LPC_TIM0->IR = 1;
					return;
				}
				else
					drawTetromino(currentX, currentY, currentRotation, currentPieceID, false);
			}			
		}
		else {
			drawTetromino(currentX, currentY, currentRotation, currentPieceID, true); // erase old position
			currentY++; // move down
			drawTetromino(currentX, currentY, currentRotation, currentPieceID, false); // draw new position
		}
		enableInterrupts();
	}
	
	LPC_TIM0->IR = 1;			/* clear interrupt flag */
	return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	// body of TIMER 1 handler
	LPC_TIM1->IR = 1;			/* clear interrupt flag */
	return;
}

/******************************************************************************
** Function name:       TIMER2_IRQHandler
** Descriptions:        Implement the conductor
******************************************************************************/
void TIMER2_IRQHandler (void) {
	// state: 0 = play note, 1 = gap (silence)
	static int musicState = 0; 
	
	if (musicState == 0) { // play phase
		uint16_t note = currentTrack[noteIndex]; 
		playNote(note); 
		LPC_TIM2->MR0 = (RHYTHM_TICK * 6) / 10; 
		
		musicState = 1; // next interrupt will be the gap
	} 
	else { // gap phase
		playNote(0); // silence the speaker
		
		LPC_TIM2->MR0 = (RHYTHM_TICK * 4) / 10;
		
		noteIndex++;
		
		if (noteIndex >= currentTrackLen) { // handle the end of track
			if (isLooping)
				noteIndex = 0; 
			else { // song over
				playNote(0); 
				LPC_TIM2->TCR = 0; // Stop conductor
				
				LPC_TIM2->MR0 = RHYTHM_TICK; 
				musicState = 0; // Reset state
				LPC_TIM2->IR = 1;
				return;
			}
		}
		
		musicState = 0; // next interrupt will be the Play Phase
	}

	LPC_TIM2->IR = 1; // Clear Timer 2 Interrupt Flag
	return;
}

/******************************************************************************
** Function name:       TIMER3_IRQHandler
** Descriptions:        Implement the oscillator
******************************************************************************/
void TIMER3_IRQHandler (void) {
	static int speakerState = 0;
		
	// toggle DAC value between 0 and mid volume
	LPC_DAC->DACR = (speakerState) ? (512 << 6) : 0; // volume
	speakerState = !speakerState;
    
	LPC_TIM3->IR = 1;
	return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
