

#include "VSL_WCH_SDI.h"


/*
1-Wire WCH SDI debug protocol for CH32V003 based on STM32F103C6 BluePill board

Detailed debug protocol described in 'QingKeV2 Microprocessor Debug Manual'

CH32V003 SDI module internal clock is 8MHz, and SDI in Normal mode 2x (slow mode) after reset

SWDIO line is open-drain and pulled up to 3.3v via a 1K resistor

The programmer signals a 1 bit with a ~300ns low pulse followed by a ~850 high pulse
and 0 bit is a ~850ns low pulse followed by  a ~850 high pulse

When reading, programmer sends 1 bit and then release a SWIO line.
The CH32V003  will hold the line low for ~800 ns to signal a 0, or or will not hold it low at all to signal a 1
I decided to wait about 300-350ns after releasing a SWIO line and the sample it.
After that I wait for a line becomes high.

After an each packet the programmer sends ~2us STOP by releasing line

SDI pin at GPIOB GPIO_PIN_9
POW pin at GPIOB GPIO_PIN_8, if used

The QingKeV2 debug protocol allows flexibility with timings.

The STM32 clock is 72Mhz, all nessesary delays made with an asm nops.
Optimization is set to 0 in compiller settings.

If you change a CPU clock or enable optimization, then all delays will change and the programmer will not work!
And you will need a logick analizer to view the actual signal timings and add/remove asm nops in a source.
Also the timings might change with a another/new compiller version.

I tried to make the delays with timers, to unbind it all from the CPU cycles, but it seems that a compiller puts a lot 
of assembler in beetweens, which eats time in an odd ways.
The best way will be to make all time-critical parts with a pure assembler, but then the sources becomes less readable, 
although the Google AI can generate quate good chunks of assemler, I decided to keep it C-based.

*/



#define SDI_IN (GPIOB->IDR & GPIO_PIN_9)


//==========================================================================

void WCH_Delay_Us(uint16_t u16Delay)
{
	uint16_t u16Tmp;

	u16Tmp = u16Delay * 72; // 72MHz
	TIM3->CNT = 0;
	while (TIM3->CNT < u16Tmp)
	{
	}
}

//==========================================================================
void WCH_Delay_Ms(uint32_t u32Delay)
{

	while (u32Delay)
	{
		WCH_Delay_Us(500);
		WCH_Delay_Us(500);
		--u32Delay;
	}
}

//==========================================================================
__attribute__((always_inline)) static inline void swio_send_one(void) {
	uint32_t n;
	  SDI_0;;
	    __asm__ volatile (
	            "nop\n\t"
	            "nop\n\t"
	            "nop\n\t"
	            "nop\n\t"
	            "nop\n\t"
	            "nop\n\t"
	            "nop\n\t"
	        );
	    __asm__ volatile (
	            "nop\n\t"
	            "nop\n\t"
	            "nop\n\t"
	            "nop\n\t"
	            "nop\n\t"
	            "nop\n\t"
	            "nop\n\t"
	        );
	  SDI_1;
	  for (n=0;n<2;n++)
	  {
		    __asm__ volatile (
		            "nop\n\t"
		            "nop\n\t"
		            "nop\n\t"
		            "nop\n\t"
		        );
	  }
}

//==========================================================================
__attribute__((always_inline)) static inline void swio_send_zero(void) {
	uint32_t n;
	  SDI_0;
	  for (n=0;n<2;n++)
	  {
		    __asm__ volatile (
		            "nop\n\t"
		            "nop\n\t"
		            "nop\n\t"
		            "nop\n\t"
		    		"nop\n\t"
		    		"nop\n\t"
		    		"nop\n\t"
		        );
	  }
	  SDI_1;
	  for (n=0;n<2;n++)
	  {
		    __asm__ volatile (
		            "nop\n\t"
		            "nop\n\t"
		            "nop\n\t"
		            "nop\n\t"
		    		"nop\n\t"
		    		"nop\n\t"
		        );
	  }
}

//==========================================================================
__attribute__((always_inline)) static inline uint32_t swio_read_bit(void)
{
	uint32_t b = 0;
	SDI_0;
    __asm__ volatile (
            "nop\n\t"
            "nop\n\t"
        );
    SDI_1;
    __asm__ volatile (
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
    		"nop\n\t"
    		"nop\n\t"
    		"nop\n\t"
    		"nop\n\t"
        );
    b = SDI_IN;
    while(!SDI_IN);
    return b;
}

//==========================================================================
void WCH_Test_Func1(void)	// unused function
{
	uint32_t b = 0;
self:
	GPIOB->BRR = GPIO_PIN_8;
    __asm__ volatile (
            "nop\n\t"
            "nop\n\t"
        );
    GPIOB->BSRR = GPIO_PIN_8;
    __asm__ volatile (
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
    		"nop\n\t"
    		"nop\n\t"
    		"nop\n\t"
    		"nop\n\t"
        );
    __asm__ volatile (
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
    		"nop\n\t"
    		"nop\n\t"
        );
    b = GPIOB->IDR & GPIO_PIN_8;
    while(!(GPIOB->IDR & GPIO_PIN_8));
    goto self;
}

//==========================================================================
void WCH_SDI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
	 /*Configure GPIO pin : PB9 */
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// reset the debug interface to "Normal mode 2x" (slow mode)
	
	SDI_1;
	WCH_Delay_Ms(5);
	SDI_0;
	WCH_Delay_Ms(20);
	SDI_1;

}

//==========================================================================
void WCH_SDI_Exit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
	 /*Configure GPIO pin : PB9 */
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

//==========================================================================
void WCH_SDI_Write(uint8_t addr, uint32_t val)
{
    volatile uint8_t i;

    // Start bit.
    swio_send_one();

    // Send the address.
    for (i = 0; i < 7; i++) {
        if (addr & 0x40)
        	swio_send_one();
        else
            swio_send_zero();

        addr <<= 1;
    }

    // send R/W bit = 1
    swio_send_one();

    // Send the word.
    for (i = 0; i < 32; i++) {
        if (val & 0x80000000)
            swio_send_one();
        else
            swio_send_zero();

        val <<= 1;
    }

    // Stop bit.
	WCH_Delay_Us(2);
}

//==========================================================================
uint32_t WCH_SDI_Read(uint8_t addr)
{
    uint8_t i;
    uint32_t x = 0;

    // Start bit.
    swio_send_one();

    // Send the address.
    for (i = 0; i < 7; i++) {
        if (addr & 0x40)
            swio_send_one();
        else
            swio_send_zero();

        addr <<= 1;
    }

    // send R/W bit 0
    swio_send_zero();

    // Receive the word.
    for (i = 0; i < 32; i++) {
        x <<= 1;

        if (swio_read_bit())
        	x |= 1;
    }

    // Stop bit.
    WCH_Delay_Us(2);

    return x;
}

