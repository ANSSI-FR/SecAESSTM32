#ifndef CORTEX_M4_SYSTICK_H
# define CORTEX_M4_SYSTICK_H

# include "helpers.h"

/**
 * systick_init - Initialize the systick module
 */
void systick_init(void);

/**
 * delay
 * @ms: Number of milliseconds to wait
 */
void delay(uint32_t ms);

/**
 * get_ticks - Get the number of milliseconds elapsed since the card boot
 * Return: Number of ticks.
 */
unsigned long long get_ticks(void);

#endif /* CORTEX_M4_SYSTICK_H */
