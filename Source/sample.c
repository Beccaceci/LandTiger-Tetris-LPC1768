#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include "button_EXINT/button.h"
#include "../game/tetris.h"
#include "../adc/adc.h"
#include "music/music.h"

#ifdef SIMULATOR
extern uint8_t ScaleFlag; 
#endif

int main(void) {
  SystemInit();
	BUTTON_init();
	NVIC_DisableIRQ(EINT1_IRQn);
	joystick_init(); 
	LCD_Initialization();
	ADC_init();
	tetris_init(); // game initialization
  
  // TIMER0 initialization: value = 1 s * 25000000 Hz = 25000000
  init_timer(0, 25000000); 

  // RIT initialization: value = 0.05 s * 100000000 Hz = 5000000
  init_RIT(5000000);
	
	NVIC_EnableIRQ(EINT1_IRQn);
  
  // power management
  LPC_SC->PCON |= 0x1;       /* power-down mode */
  LPC_SC->PCON &= ~(0x2);						
	
  while (1)	{
		__ASM("wfi");
  }
}