#include "stm32f4xx_gpio.h"
/* Used to handle EXTI events and interrupts */
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_nvic.h"
/*
** TODO: assert on param
*/


void gpio_set_mode(volatile uint32_t *gpioX_moder, uint8_t pin, uint8_t mode)
{
	set_reg_value(gpioX_moder, mode, 0x3 << (2 * pin), 2 * pin);
}

void gpio_set_type(volatile uint32_t *gpioX_otyper, uint8_t pin, uint8_t type)
{
	set_reg_value(gpioX_otyper, type, 1 << pin, pin);
}

void gpio_set_speed(volatile uint32_t *gpioX_ospeedr, uint8_t pin, uint8_t speed)
{
	set_reg_value(gpioX_ospeedr, speed, 0x3 << (2 * pin), 2 * pin);
}

void gpio_set_pupd(volatile uint32_t *gpioX_pupdr, uint8_t pin, uint8_t pupd)
{
	set_reg_value(gpioX_pupdr, pupd, 0x3 << (2 * pin), 2 * pin);
}

void gpio_set_od(volatile uint32_t *gpioX_odr, uint8_t pin, uint8_t od)
{
	set_reg_value(gpioX_odr, od, 1 << pin, pin);
}

void gpio_set_bsr_r(volatile uint32_t *gpioX_bsrr_r, uint8_t pin, uint8_t reset)
{
	set_reg_value(gpioX_bsrr_r, reset, 1 << pin, pin);
}

void gpio_set_bsr_s(volatile uint32_t *gpioX_bsrr_r, uint8_t pin, uint8_t set)
{
	set_reg_value(gpioX_bsrr_r, set, 1 << pin, pin);
}

void gpio_set_lck(volatile uint32_t *gpioX_lckr, uint8_t pin, uint8_t value)
{
	set_reg_value(gpioX_lckr, value, 1 << pin, pin);
}

void gpio_set_afr(volatile uint32_t *gpioX_afr, uint8_t pin, uint8_t function)
{
	if (pin > 7)
		set_reg_value(gpioX_afr + 1, function, 0xf << (4 * (pin - 8)), 4 * (pin - 8));
	else
		set_reg_value(gpioX_afr, function, 0xf << (4 * pin), 4 * pin);
}

#define GPIO_CONFIG(port) \
	case GPIO##port##_BASE:\
		set_reg_bits(r_CORTEX_M_RCC_AHB1ENR, RCC_AHB1ENR_GPIO##port##EN);\
		gpioX_moder = GPIO_MODER(GPIO##port##_BASE);\
		gpioX_otyper = GPIO_OTYPER(GPIO##port##_BASE);\
		gpioX_ospeedr = GPIO_OSPEEDR(GPIO##port##_BASE);\
		gpioX_pupdr = GPIO_PUPDR(GPIO##port##_BASE);\
		gpioX_idr = GPIO_IDR(GPIO##port##_BASE); \
		gpioX_odr = GPIO_ODR(GPIO##port##_BASE);\
		gpioX_bsrr_r = GPIO_BSRR_R(GPIO##port##_BASE);\
		gpioX_bsrr_s = GPIO_BSRR_S(GPIO##port##_BASE);\
		gpioX_lckr = GPIO_LCKR(GPIO##port##_BASE);\
		gpioX_afr_l = GPIO_AFR_L(GPIO##port##_BASE);\
		gpioX_afr_h = GPIO_AFR_H(GPIO##port##_BASE);\
		break;\

void gpio_set_config(const gpio_config_t *gpio)
{
	volatile uint32_t *gpioX_moder, *gpioX_otyper, *gpioX_ospeedr, *gpioX_pupdr, *gpioX_idr;
	volatile uint32_t *gpioX_odr, *gpioX_bsrr_r, *gpioX_bsrr_s, *gpioX_lckr, *gpioX_afr_l, *gpioX_afr_h;

	/* Sanity checks */
	if((gpio->port != GPIOA_BASE) && (gpio->port != GPIOB_BASE) && (gpio->port != GPIOC_BASE) &&
	   (gpio->port != GPIOD_BASE) && (gpio->port != GPIOE_BASE) && (gpio->port != GPIOF_BASE) &&
	   (gpio->port != GPIOG_BASE) && (gpio->port != GPIOH_BASE) && (gpio->port != GPIOI_BASE)){
		return;
	}
	if(gpio->pin > 15){
		return;
	}
	/* All gpio pins should be behind AHB1, so we enable the appropriate clock */
	switch(gpio->port){
		GPIO_CONFIG(A)
		GPIO_CONFIG(B)
		GPIO_CONFIG(C)
		GPIO_CONFIG(D)
		GPIO_CONFIG(E)
		GPIO_CONFIG(F)
		GPIO_CONFIG(G)
		GPIO_CONFIG(H)
		GPIO_CONFIG(I)
		default:
			/* We should not end up here */
			return;
	}
	/* Set the appropriate values according to the mask */
	if(gpio->set_mask & GPIO_SET_MODE){
		gpio_set_mode(gpioX_moder, gpio->pin, gpio->mode);
	}
	if(gpio->set_mask & GPIO_SET_TYPE){
		gpio_set_type(gpioX_otyper, gpio->pin, gpio->type);
	}
	if(gpio->set_mask & GPIO_SET_SPEED){
		gpio_set_speed(gpioX_ospeedr, gpio->pin, gpio->speed);
	}
	if(gpio->set_mask & GPIO_SET_PUPD){
		gpio_set_pupd(gpioX_pupdr, gpio->pin, gpio->pupd);
	}
	if(gpio->set_mask & GPIO_SET_OD){
		gpio_set_od(gpioX_odr, gpio->pin, gpio->od);
	}
	if(gpio->set_mask & GPIO_SET_BSR_R){
		gpio_set_bsr_r(gpioX_bsrr_r, gpio->pin, gpio->bsr_r);
	}
	if(gpio->set_mask & GPIO_SET_BSR_S){
		gpio_set_bsr_s(gpioX_bsrr_r, gpio->pin, gpio->bsr_s);
	}
	if(gpio->set_mask & GPIO_SET_LCK){
		gpio_set_lck(gpioX_lckr, gpio->pin, gpio->lck);
	}
	if(gpio->set_mask & GPIO_SET_AFR){
		gpio_set_afr(gpioX_afr_l, gpio->pin, gpio->afr);
	}

	return;
}

void gpio_set_value(const gpio_config_t *gpio, uint8_t value)
{
	gpio_set_od(GPIO_ODR(gpio->port), gpio->pin, !!value);

	return;
}

void gpio_set(const gpio_config_t *gpio)
{
	gpio_set_value(gpio, 1);

	return;
}

void gpio_clear(const gpio_config_t *gpio)
{
	gpio_set_value(gpio, 0);

	return;
}

uint8_t gpio_get(const gpio_config_t *gpio)
{
	return !!get_reg_value(GPIO_IDR(gpio->port), 1 << (gpio->pin), gpio->pin);
}


/**** IRQ Handlers ****/
#define EXTI_IRQHANDLER(num) \
/* Global variable holding the callback to USART num */\
cb_gpio_exti_handler_t cb_gpio_exti##num##_irq_handlers[9] = { NULL };\
/* Register the IRQ */\
void EXTI##num##_IRQHandler(void)\
{\
	unsigned int i;\
	for(i = 0; i < sizeof(cb_gpio_exti##num##_irq_handlers) / sizeof(cb_gpio_exti_handler_t); i++){\
	        if(cb_gpio_exti##num##_irq_handlers[i] != NULL){\
			(cb_gpio_exti##num##_irq_handlers[i])();\
		}\
	}\
}\

EXTI_IRQHANDLER(0)
EXTI_IRQHANDLER(1)
EXTI_IRQHANDLER(2)
EXTI_IRQHANDLER(3)
EXTI_IRQHANDLER(4)
EXTI_IRQHANDLER(5)
EXTI_IRQHANDLER(6)
EXTI_IRQHANDLER(7)
EXTI_IRQHANDLER(8)
EXTI_IRQHANDLER(9)
EXTI_IRQHANDLER(10)
EXTI_IRQHANDLER(11)
EXTI_IRQHANDLER(12)
EXTI_IRQHANDLER(13)
EXTI_IRQHANDLER(14)
EXTI_IRQHANDLER(15)

/* Handle aggregated EXTI (5-9 and 10-15) */
void EXTI9_5_IRQHandler(void){
	EXTI5_IRQHandler();
	EXTI6_IRQHandler();
	EXTI7_IRQHandler();
	EXTI8_IRQHandler();
	EXTI9_IRQHandler();
	return;
}

void EXTI15_10_IRQHandler(void){
	EXTI10_IRQHandler();
	EXTI11_IRQHandler();
	EXTI12_IRQHandler();
	EXTI13_IRQHandler();
	EXTI14_IRQHandler();
	EXTI15_IRQHandler();
	return;
}

/* Register an EXTI handler on an event for a given GPIO */
int gpio_register_exti_handler(const gpio_config_t *gpio, gpio_exti_events events, cb_gpio_exti_handler_t handler){
	uint8_t syscfg_exti_value = 0;
	uint32_t *syscfg_exti_ptr = NULL;
	uint8_t num_subhandler = 0;

	/* Check that the GPIO is in INPUT mode */
	if(gpio->mode != PIN_INPUT_MODE){
		goto err;
	}
	/* Sanity check */
	if(handler == NULL){
		goto err;
	}
	if(gpio->pin > 15){
		goto err;
	}

	/* Override the default handler for our GPIO.
	 * The GPIO pin number indexes the EXTI line to activate, the
	 * port indexes the bit in the register.
	 */

	switch(gpio->port){
		case GPIOA_BASE:
			syscfg_exti_value = 0b0000;
			num_subhandler = 0;
			break;
		case GPIOB_BASE:
			syscfg_exti_value = 0b0001;
			num_subhandler = 1;
			break;
		case GPIOC_BASE:
			syscfg_exti_value = 0b0010;
			num_subhandler = 2;
			break;
		case GPIOD_BASE:
			syscfg_exti_value = 0b0011;
			num_subhandler = 3;
			break;
		case GPIOE_BASE:
			syscfg_exti_value = 0b0100;
			num_subhandler = 4;
			break;
		case GPIOF_BASE:
			syscfg_exti_value = 0b0101;
			num_subhandler = 5;
			break;
		case GPIOG_BASE:
			syscfg_exti_value = 0b0110;
			num_subhandler = 6;
			break;
		case GPIOH_BASE:
			syscfg_exti_value = 0b0111;
			num_subhandler = 7;
			break;
		case GPIOI_BASE:
			syscfg_exti_value = 0b1000;
			num_subhandler = 8;
			break;
		default:
			goto err;
	}
	switch(gpio->pin){
		case 0:
			cb_gpio_exti0_irq_handlers[num_subhandler] = handler;
			NVIC_EnableIRQ(EXTI0_IRQn);
			break;
		case 1:
			cb_gpio_exti1_irq_handlers[num_subhandler] = handler;
			NVIC_EnableIRQ(EXTI1_IRQn);
			break;
		case 2:
			cb_gpio_exti2_irq_handlers[num_subhandler] = handler;
			NVIC_EnableIRQ(EXTI2_IRQn);
			break;
		case 3:
			cb_gpio_exti3_irq_handlers[num_subhandler] = handler;
			NVIC_EnableIRQ(EXTI3_IRQn);
			break;
		case 4:
			cb_gpio_exti4_irq_handlers[num_subhandler] = handler;
			NVIC_EnableIRQ(EXTI4_IRQn);
			break;
		case 5:
			cb_gpio_exti5_irq_handlers[num_subhandler] = handler;
			NVIC_EnableIRQ(EXTI9_5_IRQn);
			break;
		case 6:
			cb_gpio_exti6_irq_handlers[num_subhandler] = handler;
			NVIC_EnableIRQ(EXTI9_5_IRQn);
			break;
		case 7:
			cb_gpio_exti7_irq_handlers[num_subhandler] = handler;
			NVIC_EnableIRQ(EXTI9_5_IRQn);
			break;
		case 8:
			cb_gpio_exti8_irq_handlers[num_subhandler] = handler;
			NVIC_EnableIRQ(EXTI9_5_IRQn);
			break;
		case 9:
			cb_gpio_exti9_irq_handlers[num_subhandler] = handler;
			NVIC_EnableIRQ(EXTI9_5_IRQn);
			break;
		case 10:
			cb_gpio_exti10_irq_handlers[num_subhandler] = handler;
			NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
		case 11:
			cb_gpio_exti11_irq_handlers[num_subhandler] = handler;
			NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
		case 12:
			cb_gpio_exti12_irq_handlers[num_subhandler] = handler;
			NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
		case 13:
			cb_gpio_exti13_irq_handlers[num_subhandler] = handler;
			NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
		case 14:
			cb_gpio_exti14_irq_handlers[num_subhandler] = handler;
			NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
		case 15:
			cb_gpio_exti15_irq_handlers[num_subhandler] = handler;
			NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
		default:
			goto err;
	}
	switch((gpio->pin) / 4){
		case 0:
			/* SYSCFG_EXTICR1 */
			syscfg_exti_ptr = SYSCFG_EXTICR1;
			break;
		case 1:
			/* SYSCFG_EXTICR2 */
			syscfg_exti_ptr = SYSCFG_EXTICR2;
			break;
		case 2:
			/* SYSCFG_EXTICR3 */
			syscfg_exti_ptr = SYSCFG_EXTICR3;
			break;
		case 3:
			/* SYSCFG_EXTICR4 */
			syscfg_exti_ptr = SYSCFG_EXTICR4;
			break;
		default:
			goto err;
	}
	/* Enable the EXTI line in the proper SYSCFG EXTI register */
	set_reg_value(syscfg_exti_ptr, syscfg_exti_value, 0b1111, (gpio->pin % 4));

	/* Enable the interrupt line in EXTI_IMR */
	set_reg_bits(EXTI_IMR, (0x1 << gpio->pin));

	/* Depending on the type of edge we want to trigger on, configure the registers */
	switch(events){
		case GPIO_EXTI_RISING:
			set_reg_bits(EXTI_RTSR, (0x1 << gpio->pin));
			break;
		case GPIO_EXTI_FALLING:
			set_reg_bits(EXTI_FTSR, (0x1 << gpio->pin));
			break;
		case GPIO_EXTI_RISING_FALLING:
			set_reg_bits(EXTI_RTSR, (0x1 << gpio->pin));
			set_reg_bits(EXTI_FTSR, (0x1 << gpio->pin));
			break;
		default:
			goto err;
	}
	
	return 0;
err:
	return -1;
}
