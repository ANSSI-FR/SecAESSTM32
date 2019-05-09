#include "leds.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

/* Our LED definitions */
#define LED_GPIO_COMMON(po, pi) \
        {\
         .port = po,\
         .pin = pi,\
         .set_mask = GPIO_SET_MODE | GPIO_SET_PUPD | GPIO_SET_SPEED | GPIO_SET_TYPE,\
         .mode = PIN_OUTPUT_MODE,\
         .pupd = GPIO_PULLDOWN,\
         .speed = PIN_HIGH_SPEED,\
         .type = PIN_OTYPER_PP,\
        }\

const gpio_config_t leds[] = {
        /* LED0 */
        LED_GPIO_COMMON(GPIOD_BASE, 4),
        /* LED1 */
        LED_GPIO_COMMON(GPIOD_BASE, 5),
        /* LED3 */
        LED_GPIO_COMMON(GPIOD_BASE, 13),
        /* LED4 */
        LED_GPIO_COMMON(GPIOD_BASE, 12),
        /* LED5 */
        LED_GPIO_COMMON(GPIOD_BASE, 14),
        /* LED6 */
        LED_GPIO_COMMON(GPIOD_BASE, 15),
};

void leds_init(void)
{
 	gpio_set_config(&leds[LED3]);
 	gpio_set_config(&leds[LED4]);
 	gpio_set_config(&leds[LED5]);
 	gpio_set_config(&leds[LED6]);
}

void leds_on(enum led led)
{
	gpio_set_value(&leds[led], 1);
}
void leds_off(enum led led)
{
	gpio_set_value(&leds[led], 0);
}
void leds_toggle(enum led led)
{
	gpio_set_value(&leds[led], !!gpio_get(&leds[led]));
}

void leds_blinky(void)
{
  
  leds_on(LED3);
  leds_on(LED4);
  leds_on(LED5);
  leds_on(LED6);
  
  sleep_intern(MEDIUM_TIME);

  leds_off(LED3);
  leds_off(LED4);
  leds_off(LED5);
  leds_off(LED6);

  sleep_intern(LONG_TIME);
}
