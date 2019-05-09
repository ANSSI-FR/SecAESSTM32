#ifndef STM32F4XX_SYSCFG_H
#define STM32F4XX_SYSCFG_H

#include "stm32f4xx_core.h"

#define SYSCFG_MEMRMP  ((uint32_t*)(SYSCFG_BASE+0x0))
#define SYSCFG_PMC     ((uint32_t*)(SYSCFG_BASE+0x4))
#define SYSCFG_EXTICR1 ((uint32_t*)(SYSCFG_BASE+0x8))
#define SYSCFG_EXTICR2 ((uint32_t*)(SYSCFG_BASE+0x0C))
#define SYSCFG_EXTICR3 ((uint32_t*)(SYSCFG_BASE+0x10))
#define SYSCFG_EXTICR4 ((uint32_t*)(SYSCFG_BASE+0x14))
#define SYSCFG_CMPCR   ((uint32_t*)(SYSCFG_BASE+0x20))


#endif /* STM32F4XX_SYSCFG_H */
