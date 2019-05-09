#include "cortex_m4_systick.h"
#include "cortex_m4_systick_regs.h"
#include "helpers.h"
#include "product.h"

static volatile unsigned long long ticks;

void SysTick_Handler(void)
{
	ticks++;
}

void systick_init(void)
{
	set_reg(r_CORTEX_M_STK_LOAD, PROD_CORE_FREQUENCY , STK_RELOAD);
	set_reg(r_CORTEX_M_STK_VAL, 0, STK_CURRENT);
	set_reg_bits(r_CORTEX_M_STK_CTRL, STK_CLKSOURCE_Msk | STK_TICKINT_Msk | STK_ENABLE_Msk);
}

void delay(uint32_t ms)
{
	unsigned long long start = ticks;
	while (start + ms > ticks)
		continue;
}

unsigned long long get_ticks(void)
{
	return ticks;
}
