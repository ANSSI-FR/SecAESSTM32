#ifndef PRODUCT_H
# define PRODUCT_H
#include "stm32f4xx_rcc.h"
#define VECT_TAB_OFFSET  0x00 /*!< Vector Table base offset field.
                                  This value must be a multiple of 0x200. */


#define PROD_PLL_M     8
#define PROD_PLL_N     168
#define PROD_PLL_P     2
#define PROD_PLL_Q     7

#define PROD_HCLK      RCC_CFGR_HPRE_DIV1
#define PROD_PCLK2     RCC_CFGR_HPRE2_DIV2
#define PROD_PCLK1     RCC_CFGR_HPRE1_DIV4

#define PROD_CLOCK_APB1  42000000 // MHz
#define PROD_CLOCK_APB2  84000000 // MHz
#define PROD_CONSOLE_USART 1
#define PROD_PROTOCOL_USART 4

#define PROD_CORE_FREQUENCY 168000

#define VTORS_SIZE 	0x188

#endif /* PRODUCT_H */
