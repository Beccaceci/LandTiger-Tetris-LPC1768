/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "../game/tetris.h"
#include "../game/graphics.h"
#include "../game/pieces.h"

#define J_DOWN   (1 << 26)
#define J_LEFT   (1 << 27)
#define J_RIGHT  (1 << 28)
#define J_UP     (1 << 29)

static int JLeftPressed = 0;
static int JRightPressed = 0;
static int JDownPressed = 0;
static int JUpPressed = 0;

static int downCounter = 0;


/**
 * @brief Disables T0 and button interrupts.
*/
static void disableInterrupts (void) {
	NVIC_DisableIRQ(TIMER0_IRQn);
	NVIC_DisableIRQ(EINT1_IRQn);
	NVIC_DisableIRQ(EINT2_IRQn);
}


/**
 * @brief Re-enables all critical interrupts.
*/
static void enableInterrupts (void) {
	NVIC_EnableIRQ(TIMER0_IRQn);
	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_EnableIRQ(EINT2_IRQn);
}


/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void RIT_IRQHandler(void) {
		if (currentState == STATE_PLAYING) {
        /* ---------------- JOYSTICK LEFT ---------------- */
        if ((LPC_GPIO1->FIOPIN & J_LEFT) == 0) {
          if (JLeftPressed == 0) {
            disableInterrupts();
          
            if (checkCollision(currentX - 1, currentY, currentRotation, currentPieceID) == false) { // check collision for X-1
              drawTetromino(currentX, currentY, currentRotation, currentPieceID, true); // erase old position
              currentX--; // update position
              drawTetromino(currentX, currentY, currentRotation, currentPieceID, false); // draw new position
            }
            
            enableInterrupts();
            JLeftPressed = 1;
          }
        }
				else
          JLeftPressed = 0; // reset when released

        /* ---------------- JOYSTICK RIGHT ---------------- */
        if ((LPC_GPIO1->FIOPIN & J_RIGHT) == 0) {
          if (JRightPressed == 0) {
            disableInterrupts();
          
            if (checkCollision(currentX + 1, currentY, currentRotation, currentPieceID) == false) { // check for collision X+1
              drawTetromino(currentX, currentY, currentRotation, currentPieceID, true);
              currentX++;
              drawTetromino(currentX, currentY, currentRotation, currentPieceID, false);
            }
            
            enableInterrupts();
            JRightPressed = 1;
          }
        }
				else
          JRightPressed = 0;
				
				/* ---------------- JOYSTICK UP ---------------- */
        if ((LPC_GPIO1->FIOPIN & J_UP) == 0) {
          if (JUpPressed == 0) {
            disableInterrupts();
          
            int nextRotation = (currentRotation + 1) % NUM_ROTATIONS;
            if (checkCollision(currentX, currentY, nextRotation, currentPieceID) == false) { // check if the new rotation fits
              drawTetromino(currentX, currentY, currentRotation, currentPieceID, true); // erase old rotation
              currentRotation = nextRotation;
              drawTetromino(currentX, currentY, currentRotation, currentPieceID, false); // draw new rotation
            } 
            
            enableInterrupts();
            JUpPressed = 1;
          }
        }
        else
          JUpPressed = 0;
        
        /* ---------------- JOYSTICK DOWN ---------------- */
        // this scenario is now handled by TIMER 0 handler 
    }

    LPC_RIT->RICTRL |= 0x1; /* Clear RIT Interrupt Flag */
}

/******************************************************************************
**                            End Of File
******************************************************************************/
