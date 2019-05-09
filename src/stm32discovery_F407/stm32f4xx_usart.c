#include "debug.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_nvic.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_usart_regs.h"


/**** USART basic Read / Write ****/
void usart_putc(uint8_t usart, uint16_t c)
{
	/* Wait for TX to be ready */
	while (!get_reg(r_CORTEX_M_USART_SR(usart), USART_SR_TXE))
		continue;
	*r_CORTEX_M_USART_DR(usart) = c;
}
/* Instantiate the putc for each USART */
#define USART_PUTC_CALLBACK(num)\
void usart##num##_putc(uint16_t c)\
{\
	usart_putc(num, c);\
}\

USART_PUTC_CALLBACK(1)
USART_PUTC_CALLBACK(2)
USART_PUTC_CALLBACK(3)
USART_PUTC_CALLBACK(4)
USART_PUTC_CALLBACK(5)
USART_PUTC_CALLBACK(6)

void usart_write(uint8_t usart, char *msg, uint32_t len)
{
	while (len--) {
		usart_putc(usart, *msg);
		msg++;
	}
}

uint16_t usart_getc(uint8_t usart)
{
	while (!get_reg(r_CORTEX_M_USART_SR(usart), USART_SR_RXNE))
		continue;
	return *r_CORTEX_M_USART_DR(usart);
}
/* Instantiate the getc for each USART */
#define USART_GETC_CALLBACK(num)\
uint16_t usart##num##_getc(void)\
{\
	return usart_getc(num);\
}\

USART_GETC_CALLBACK(1)
USART_GETC_CALLBACK(2)
USART_GETC_CALLBACK(3)
USART_GETC_CALLBACK(4)
USART_GETC_CALLBACK(5)
USART_GETC_CALLBACK(6)



uint32_t usart_read(uint8_t usart, char *buf, uint32_t len)
{
	uint32_t start_len = len;
	while (len--) {
		*buf = usart_getc(usart);
		if (*buf == '\n')
			break;
		buf++;
	}
	return start_len - len;
}

/**** IRQ Handlers ****/
#define USART_IRQHANDLER(num, type) \
/* Global variable holding the callback to USART num */\
cb_usart_irq_handler_t cb_usart##num##_irq_handler = NULL;\
/* Register the IRQ */\
void U##type##ART##num##_IRQHandler(void)\
{\
	if(cb_usart##num##_irq_handler != NULL){\
		cb_usart##num##_irq_handler();\
	}\
}\

/* Instantiate the IRQs for the 6 USARTs
 * The weird second macro argument handles the fact that USART 4 and 5 are in
 * UARTs.
 */
USART_IRQHANDLER(1,S)
USART_IRQHANDLER(2,S)
USART_IRQHANDLER(3,S)
USART_IRQHANDLER(4,)
USART_IRQHANDLER(5,)
USART_IRQHANDLER(6,S)

/* Configure the handlers */
#define USART_CONFIG_CALLBACKS(num, type) \
	case num:\
		if (config->callback_irq_handler != NULL){\
			NVIC_EnableIRQ(U##type##ART##num##_IRQn);\
			/* Enable dedicated IRQ and register the callback */\
			cb_usart##num##_irq_handler = config->callback_irq_handler;\
		}\
		if(config->callback_usart_getc_ptr != NULL){\
			*(config->callback_usart_getc_ptr) = usart##num##_getc;\
		}\
		if(config->callback_usart_putc_ptr != NULL){\
			*(config->callback_usart_putc_ptr) = usart##num##_putc;\
		}\
		break;\

static void usart_callbacks_init(usart_config_t *config)
{
	switch (config->usart){
		USART_CONFIG_CALLBACKS(1,S)
		USART_CONFIG_CALLBACKS(2,S)
		USART_CONFIG_CALLBACKS(3,S)
		USART_CONFIG_CALLBACKS(4,)
		USART_CONFIG_CALLBACKS(5,)
		USART_CONFIG_CALLBACKS(6,S)
		default:
			panic("Wrong usart %d. You should use USART1 to USART6", config->usart);
	}


	return;
}

/**** DMA ****/
void usart_disable_dma(uint8_t usart)
{
	clear_reg_bits(r_CORTEX_M_USART_CR3(usart), USART_CR3_DMAT_Msk);
}

void usart_enable_dma(uint8_t usart)
{
	set_reg_bits(r_CORTEX_M_USART_CR3(usart), USART_CR3_DMAT_Msk);
}

volatile uint32_t *usart_get_data_addr(uint8_t usart)
{
	return r_CORTEX_M_USART_DR(usart);
}

void usart_set_baudrate(usart_config_t *config)
{
	uint32_t divider = 0;
	uint16_t mantissa = 0;
	uint8_t fraction = 0;

	/* FIXME we should check CR1 in order to get the OVER8 configuration */

	/* Compute the divider using the baudrate and the APB bus clock 
	 * (APB1 or APB2) depending on the considered USART */
	divider = usart_get_bus_clock(config) / config->baudrate;

	mantissa = (uint16_t)divider / 16;
	fraction = (uint8_t)((divider - mantissa * 16));

	write_reg_value(r_CORTEX_M_USART_BRR(config->usart), ( ((mantissa & 0x0fff) << 4) | (0x0f & fraction)) );
}


/* USART mapping for UART mode (TX, RX), and their configuration */
#define UART_GPIO_COMMON(po, pi, x) \
	{\
	 .port = po,\
	 .pin = pi,\
	 .set_mask = GPIO_SET_ALL,\
	 .mode = PIN_ALTERNATE_MODE,\
	 .pupd = GPIO_NOPULL,\
	 .speed = PIN_VERY_HIGH_SPEED,\
	 .type = PIN_OTYPER_PP,\
	 .afr = x,\
	}\

const gpio_config_t uart_gpio_config[] = {
	/***************************************/
	/* USART1 */
	/* TX is PB6 with alternate function AF7 */
	UART_GPIO_COMMON(GPIOB_BASE, 6, GPIO_AF_USART1),
	/* RX is PB7 with alternate function AF7 */
	UART_GPIO_COMMON(GPIOB_BASE, 7, GPIO_AF_USART1),
	/***************************************/
	/* USART2 */
	/* TX is PA2 with alternate function AF7 */
	UART_GPIO_COMMON(GPIOA_BASE, 2, GPIO_AF_USART2),
	/* RX is PA3 with alternate function AF7 */
	UART_GPIO_COMMON(GPIOA_BASE, 3, GPIO_AF_USART2),
	/***************************************/
	/* USART3 */
	/* TX is PB10 with alternate function AF7 */
	UART_GPIO_COMMON(GPIOB_BASE, 10, GPIO_AF_USART3),
	/* RX is PB11 with alternate function AF7 */
	UART_GPIO_COMMON(GPIOB_BASE, 11, GPIO_AF_USART3),
	/***************************************/
	/* USART4 */
	/* TX is PC10 with alternate function AF8 */
	UART_GPIO_COMMON(GPIOC_BASE, 10, GPIO_AF_USART4),
	/* RX is PC11 with alternate function AF8 */
	UART_GPIO_COMMON(GPIOC_BASE, 11, GPIO_AF_USART4),
	/***************************************/
	/* USART5 */
	/* TX is PC12 with alternate function AF8 */
	UART_GPIO_COMMON(GPIOC_BASE, 12, GPIO_AF_USART5),
	/* RX is PD2 with alternate function AF8 */
	UART_GPIO_COMMON(GPIOD_BASE, 2, GPIO_AF_USART5),
	/***************************************/
	/* USART6 */
	/* TX is PC6 with alternate function AF8 */
	UART_GPIO_COMMON(GPIOC_BASE, 6, GPIO_AF_USART6),
	/* RX is PC7 with alternate function AF8 */
	UART_GPIO_COMMON(GPIOC_BASE, 7, GPIO_AF_USART6),
};

/* This is a template configuration for the USART in UART mode */


/* USART mapping for SMARTCARD mode (TX, CK), and their configuration */
#define SMARTCARD_GPIO_COMMON_TX(po, pi, x) \
	{\
	 .port = po,\
	 .pin = pi,\
	 .set_mask = GPIO_SET_ALL,\
	 .mode = PIN_ALTERNATE_MODE,\
	 .pupd = GPIO_PULLUP,\
	 .speed = PIN_VERY_HIGH_SPEED,\
	 .type = PIN_OTYPER_OD,\
	 .afr = x,\
	}\

#define SMARTCARD_GPIO_COMMON_CK(po, pi, x) \
	{\
	 .port = po,\
	 .pin = pi,\
	 .set_mask = GPIO_SET_ALL,\
	 .mode = PIN_ALTERNATE_MODE,\
	 .pupd = GPIO_PULLUP,\
	 .speed = PIN_VERY_HIGH_SPEED,\
	 .type = PIN_OTYPER_PP,\
	 .afr = x,\
	}\

const gpio_config_t smartcard_gpio_config[] = {
	/***************************************/
	/* USART1 */
	/* TX is PA9 with alternate function AF7 */
	SMARTCARD_GPIO_COMMON_TX(GPIOA_BASE, 9, GPIO_AF_USART1),
	/* CK is PA8 with alternate function AF7 */
	SMARTCARD_GPIO_COMMON_CK(GPIOA_BASE, 8, GPIO_AF_USART1),
	/***************************************/
	/* USART2 */
	/* TX is PA2 with alternate function AF7 */
	SMARTCARD_GPIO_COMMON_TX(GPIOA_BASE, 2, GPIO_AF_USART2),
	/* CK is PA4 with alternate function AF7 */
	SMARTCARD_GPIO_COMMON_CK(GPIOA_BASE, 4, GPIO_AF_USART2),
	/***************************************/
	/* USART3 */
	/* TX is PB10 with alternate function AF7 */
	SMARTCARD_GPIO_COMMON_TX(GPIOB_BASE, 10, GPIO_AF_USART3),
	/* CK is PB12 with alternate function AF7 */
	SMARTCARD_GPIO_COMMON_CK(GPIOB_BASE, 12, GPIO_AF_USART3),
	/***************************************/
	/* USART4 does not support smartcard mode */
	SMARTCARD_GPIO_COMMON_TX(0, 0, 0),
	SMARTCARD_GPIO_COMMON_CK(0, 0, 0),
	/***************************************/
	/* USART5 does not support smartcard mode */
	SMARTCARD_GPIO_COMMON_TX(0, 0, 0),
	SMARTCARD_GPIO_COMMON_CK(0, 0, 0),
	/***************************************/
	/* USART6 */
	/* TX is PC6 with alternate function AF8 */
	SMARTCARD_GPIO_COMMON_TX(GPIOC_BASE, 6, GPIO_AF_USART6),
	/* CK is PC8 with alternate function AF8 */
	SMARTCARD_GPIO_COMMON_CK(GPIOC_BASE, 8, GPIO_AF_USART6),
};

/* FIXME should be rewritten in order to handle all uart configurations 
 * [RB]: first attempt to implement this ...
 */
static void init_gpio(usart_config_t *config)
{
	/* Sanity check */
	if((config->usart < 1) || (config->usart > 6)){
            panic("Wrong usart %d. You should use USART1 to USART6", config->usart);	
	}
	switch(config->mode){
		case UART:
		{
			const gpio_config_t *tx_config = &uart_gpio_config[2*(config->usart-1)];
			const gpio_config_t *rx_config = &uart_gpio_config[(2*(config->usart-1)) + 1];
			if((tx_config->port == 0) || (rx_config->port == 0)){
            			panic("UART usart %d does not seem to support UART or to be rooted on the board!", config->usart);
			}
			/* Configure with proper TX and RX */
			gpio_set_config(tx_config);
			gpio_set_config(rx_config);
			/* Configure the */
			break;
		}
		case SMARTCARD:
		{
			const gpio_config_t *tx_config = &smartcard_gpio_config[2*(config->usart-1)];
			const gpio_config_t *ck_config = &smartcard_gpio_config[(2*(config->usart-1)) + 1];
			if((tx_config->port == 0) || (ck_config->port == 0)){
            			panic("Usart %d does not seem to support smartcard mode, or is not rooted on the board!", config->usart);
			}
			/* Configure with proper TX and CK */
			gpio_set_config(tx_config);
			gpio_set_config(ck_config);
			break;
		}
		default:
            		panic("Wrong usart mode %d.", config->mode);
	}
}

static void usart_clock_init(usart_config_t *config)
{
	switch (config->usart){
		case 1:
			set_reg_bits(r_CORTEX_M_RCC_APB2ENR, RCC_APB2ENR_USART1EN);
            		break;
		case 2:
			set_reg_bits(r_CORTEX_M_RCC_APB1ENR, RCC_APB1ENR_USART2EN);
			break;
		case 3:
			set_reg_bits(r_CORTEX_M_RCC_APB1ENR, RCC_APB1ENR_USART3EN);
			break;
		case 4:
			set_reg_bits(r_CORTEX_M_RCC_APB1ENR, RCC_APB1ENR_UART4EN);
			break;
		case 5:
			set_reg_bits(r_CORTEX_M_RCC_APB1ENR, RCC_APB1ENR_UART5EN);
			break;
		case 6:
			set_reg_bits(r_CORTEX_M_RCC_APB2ENR, RCC_APB2ENR_USART6EN);
			break;
		default :
			panic("Wrong usart %d. You should use USART1 to USART6", config->usart);
	}
	
	return;
}


/**** usart_init ****/
void usart_init(usart_config_t *config)
{
	usart_clock_init(config);
	init_gpio(config);
	usart_set_baudrate(config);

	/* Enable the USART */
	usart_enable(config);

	/* Control register 1 */
	set_reg(r_CORTEX_M_USART_CR1(config->usart), config->parity, USART_CONFIG_PARITY);
	set_reg(r_CORTEX_M_USART_CR1(config->usart), config->word_length, USART_CONFIG_WORD_LENGTH_BITS);
	set_reg(r_CORTEX_M_USART_CR1(config->usart), config->options_cr1, USART_CONFIG_OPTIONS_CR1);

	/* Control register 2 */
	set_reg(r_CORTEX_M_USART_CR2(config->usart), config->stop_bits, USART_CONFIG_STOP_BITS);
	set_reg(r_CORTEX_M_USART_CR2(config->usart), config->options_cr2, USART_CONFIG_OPTIONS_CR2);

	/* USART 4 and 5 have some configuration limitations: check them before continuing */
	if((config->hw_flow_control & (USART_CR3_CTSIE_Msk | USART_CR3_CTSE_Msk | USART_CR3_RTSE_Msk | USART_CR3_SCEN_Msk | USART_CR3_NACK_Msk)) 
	   && ((config->usart == 4) || (config->usart == 5))){
		panic("Usart%d config error: asking for a flag in CR3 unavailable for USART4 and USART5", config->usart);
	}
	if((config->hw_flow_control & (USART_CR3_DMAT_Msk | USART_CR3_DMAR_Msk))
	   && (config->usart == 5)){
		panic("Usart%d config error: asking for a flag in CR3 unavailable for USART5", config->usart);
	}
	/* Control register 3 */
	set_reg(r_CORTEX_M_USART_CR3(config->usart), config->hw_flow_control, USART_CONFIG_HW_FLW_CTRL);

	if((config->guard_time_prescaler) 
	    && ((config->usart == 4) || (config->usart == 5))){
		panic("Usart%d config error: asking for guard time/prescaler in GTPR unavailable for USART4 and USART5", config->usart);
	}
	/* Prescaler and guard time */
	set_reg(r_CORTEX_M_USART_GTPR(config->usart), config->guard_time_prescaler, USART_CONFIG_GUARD_TIME_PRESCALER);

	/* Clear necessary bits */
	clear_reg_bits(r_CORTEX_M_USART_SR(config->usart), USART_SR_TC_Msk);
	clear_reg_bits(r_CORTEX_M_USART_SR(config->usart), USART_SR_RXNE_Msk);
	clear_reg_bits(r_CORTEX_M_USART_SR(config->usart), USART_SR_CTS_Msk);
	clear_reg_bits(r_CORTEX_M_USART_SR(config->usart), USART_SR_LIN_Msk);

	/* Initialize callbacks */
	usart_callbacks_init(config);

	return;
}

/* Get the current clock value of the USART bus */
uint32_t usart_get_bus_clock(usart_config_t *config)
{
	switch (config->usart){
		case 1:
		case 6:
			return PROD_CLOCK_APB2;
            		break;
		case 2:
		case 3:
		case 4:
		case 5:
			return PROD_CLOCK_APB1;
            		break;
		default :
			panic("Wrong usart %d. You should use USART1 to USART6", config->usart);
	}
	
	return 0;
}

/*
 * usart_enable - Enable the specific USART
 */
void usart_enable(usart_config_t *config)
{
	set_reg_bits(r_CORTEX_M_USART_CR1(config->usart), USART_CR1_UE_Msk);

	return;
}

/*
 * usart_disable - Disable the specific USART
 */
void usart_disable(usart_config_t *config)
{
	clear_reg_bits(r_CORTEX_M_USART_CR1(config->usart), USART_CR1_UE_Msk);

	return;
}
