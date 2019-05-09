#ifndef LEDS_H_
# define LEDS_H_

#include "helpers.h"
#include "stm32f4xx_gpio.h"

enum led {LED0 = 0, LED1 = 1, LED3 = 2, LED4 = 3, LED5 = 4, LED6 = 5};

/** 
 * leds_init - init the led module
 */
void leds_init(void);
/**
 * leds_on - turn on a led
 * @led: which led to turn on
 */
void leds_on(enum led led);
/**
 * leds_off - turn off a led
 * @led: which led to turn off
 */
void leds_off(enum led led);
/**
 * leds_toggle - turn toggle a led
 * @led: which led to turn toggle
 */
void leds_toggle(enum led led);
/**
 * leds_blinky - blink every led (discovery only)
 */
void leds_blinky(void);
#endif /* !LEDS_H_ */
