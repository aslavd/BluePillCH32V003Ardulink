

#ifndef INC_VSL_WCH_SDI_H_
#define INC_VSL_WCH_SDI_H_

#include "stm32f1xx_hal.h"

#define SDI_1 GPIOB->BSRR = GPIO_PIN_9
#define SDI_0 GPIOB->BRR =  GPIO_PIN_9

#define WCH_SDI_DATA0 0x04
#define WCH_SDI_DATA1 0x05
#define WCH_SDI_DMCONTROL 0x10
#define WCH_SDI_DMSTATUS 0x11
#define WCH_SDI_HARTINFO 0x12
#define WCH_SDI_ABSTRACTCS 0x16
#define WCH_SDI_COMMAND 0x17
#define WCH_SDI_ABSTRACTAUTO 0x18
#define WCH_SDI_PROGBUFF0 0x20
#define WCH_SDI_PROGBUFF1 0x21
#define WCH_SDI_PROGBUFF2 0x22
#define WCH_SDI_PROGBUFF3 0x23
#define WCH_SDI_PROGBUFF4 0x24
#define WCH_SDI_PROGBUFF5 0x25
#define WCH_SDI_PROGBUFF6 0x26
#define WCH_SDI_PROGBUFF7 0x27
#define WCH_SDI_HALTSUM0 0x40

#define WCH_SDI_CPBR 0x7C
#define WCH_SDI_CFGR 0x7D
#define WCH_SDI_SHDWCFGR 0x7E

#define R32_FLASH_BASE 0x40022000
#define R32_FLASH_ACTLR 0x40022000
#define R32_FLASH_KEYR 0x40022004
#define R32_FLASH_OBKEYR 0x40022008
#define R32_FLASH_STATR 0x4002200C
#define R32_FLASH_CTLR 0x40022010
#define R32_FLASH_ADDR 0x40022014
#define R32_FLASH_OBR 0x4002201C
#define R32_FLASH_WPR 0x40022020
#define R32_FLASH_MODEKEYR 0x40022024
#define R32_FLASH_BOOT_MODEKEYR 0x40022028

void WCH_SDI_Init(void);
void WCH_SDI_Exit(void);
void WCH_SDI_Write(uint8_t addr, uint32_t val);
uint32_t WCH_SDI_Read(uint8_t addr);
void WCH_Delay_Ms(uint32_t u32Delay);
void WCH_Delay_Us(uint16_t u16Delay);

void WCH_Test_Func(void);

//uint32_t NHC_WchSdi_WriteMem(uint32_t u32Address, uint32_t u32Data, uint8_t u8Width);
//uint32_t NHC_WchSdi_ReadMem(uint32_t u32Address, uint32_t *pu32Data, uint8_t u8Width);


#endif /* INC_VSL_WCH_SDI_H_ */
