#ifndef _STM32F4XX_LAYOUT_H
#define _STM32F4XX_LAYOUT_H

    	#define NB_MEM_BANK 	1

    	#define FLASH_SIZE 	1024K
    	#define VTORS_SIZE 	0x188

	#define FLASH_BASE	0x08000000
	#define RAM_BASE  	0x20000000
	#define RAM2_BASE  	0x10000000
	#define MB1_BASE  	0x60000000
    	#define MB2_BASE  	0x60000000

	#define RAM_SIZE   	128K
	#define RAM_SIZE_  	(128 * 1024)
	#define RAM2_SIZE   	64K
	#define RAM2_SIZE_  	(64 * 1024)
	#define MB1_SIZE    	0
    	#define MB2_SIZE    	0

#endif /*  _STM32F4XX_LAYOUT_H */



