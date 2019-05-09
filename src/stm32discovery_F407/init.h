#ifndef INIT_H
# define INIT_H

# include "helpers.h"
# include "product.h"
# include "stm32f4xx_rcc.h"

# define RESET	0
# define SET	1

# if !defined (HSE_STARTUP_TIMEOUT)
#  define HSE_STARTUP_TIMEOUT	((uint16_t)0x0500)
# endif /* !HSE_STARTUP_TIMEOUT */
# if !defined (HSI_STARTUP_TIMEOUT)
#  define HSI_STARTUP_TIMEOUT	((uint16_t)0x0500)
# endif /* !HSI_STARTUP_TIMEOUT */
void SetSysClock(void);
void set_vtor(uint32_t);
void system_init(uint32_t);

#endif
