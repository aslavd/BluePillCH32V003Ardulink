This is an Ardulink-compatible CH32V003 programmer ported to the cheap STM32F103C6 BluePill board.
It uses an USB CDC (COM Port) from the STM32 HAL Drivers, so you can connect it directly to an USB 
without any converter, it appears as a COM port in hardware. Tested with minichlink, work fine, althought 
it should be checked with a at least 10ns logick analizer to see exact timings, which I dont have.
I checked it with a ~50+ns scope, the SDI timings looks suitable from far away. 
As for a STM32F103C8 BluePills, you can flash the firmware and it should work (Google AI says it). 
The firmware file is Test03.bin (and Test03.elf) in Debug folder. 

SDIO line is on the GPIOB9 pin (PB9), POW_ON line (if used) is on the GPIOB8 pin (PB8).
You must solder a 1K pull-up resistor between the PB9 pin and 3V3 on a BluePill board, or it will not work.
Build-in BluePill LED (on PC13) is ON during any USB CDC read/write operations. 
An internal BluePill 3V3 power used to power CH32V003 device. If you don't use the SDIO pin on CH32V003, then 
only 3 wires are enough: 3V3, GND, SDIO.

The project is made with the STM32CubeIDE 1.7.0 and FW_F1 1.8.0 without any tweaking of the standart 
HAL/Middlewares or external libraries. 

If you want to change sources, be aware, that all delays are made with an asm("nop") and because of that dependent 
on the SYSCLK which is set to (exactly!) 72MHz and an Optimization disabled (set to 0) in compiller setting.
The delays might probably change for other compilers, "gnu-tools-for-stm32.9-2020-q2-update" used here. 
The STM32F103C6 32K flash is always full, consider a STM32F103C8 based BluePill for extending.

Anyways, if you don't want to dig in the sources, simple flash Test03.bin/elf from "Debug" folder to a BluePill 
with the STM32CubeProgrammer and it will work.

Thanks to NHC-Link042 and Ardulink-pio for theirs work.

