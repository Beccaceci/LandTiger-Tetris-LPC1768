/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_adc.c
** Last modified Date:  20184-12-30
** Last Version:        V1.00
** Descriptions:        functions to manage A/D interrupts
** Correlated files:    adc.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "LPC17xx.h"
#include "adc.h"

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

volatile unsigned short AD_current = 0;

void ADC_IRQHandler(void) {
  AD_current = ((LPC_ADC->ADGDR >> 4) & 0xFFF); 

  static unsigned short AD_last = 0xFF;
  if (AD_current != AD_last)
		AD_last = AD_current;
}
