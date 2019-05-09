#include <stdarg.h>
#include "debug.h"
#include "helpers.h"
#include "product.h"
#include "stm32f4xx_usart.h"
#include "printf.h"
#include "string.h"

static volatile int logging = 1;

cb_usart_getc_t console_getc = NULL;
cb_usart_putc_t console_putc = NULL;

static uint32_t current_dbg_usart = 0;
void cb_console_data_irq(void)
{
	if(get_reg(r_CORTEX_M_USART_SR(current_dbg_usart), USART_SR_RXNE)){
		/* Data received */
		char c;
		if(console_getc == NULL){
			panic("Error: console_getc not initialized!");
		}
	
		c = console_getc();

		if (c != ' ' && c != 'p') {
			dbg_flush();
			return;
		}

		logging = !logging;
		if (logging){
			dbg_log("Logging enabled\n");
		}
		else{
			dbg_log("Logging disabled\n");
		}
		dbg_flush();
	}
}

void debug_console_init(usart_config_t *config){

    /* Configure the USART in UART mode */
    if (config != NULL){
        config->set_mask = USART_SET_ALL;
        config->usart = PROD_CONSOLE_USART;
        config->baudrate = 115200;
        config->word_length = USART_CR1_M_8;
        config->stop_bits = USART_CR2_STOP_1BIT;
        config->parity = USART_CR1_PCE_DIS;
        config->hw_flow_control = USART_CR3_CTSE_CTS_DIS | USART_CR3_RTSE_RTS_DIS;
	/* We disable the TX interrupt since we will handle it with polling */
        config->options_cr1 = USART_CR1_TE_EN | USART_CR1_RE_EN | USART_CR1_UE_EN | USART_CR1_RXNEIE_EN | USART_CR1_TCIE_DIS;
        if(config->callback_irq_handler == NULL){
            config->callback_irq_handler = cb_console_data_irq;
        }
        if(config->callback_usart_getc_ptr == NULL){
	    config->callback_usart_getc_ptr = &console_getc;
        }
        if(config->callback_usart_putc_ptr == NULL){
   	    config->callback_usart_putc_ptr = &console_putc;
        }
	current_dbg_usart = PROD_CONSOLE_USART;
    }
    else{
        return;
    }
    /* Initialize the USART related to the console */
    usart_init(config);
    dbg_log("[USART%d initialized for console output, baudrate=%d]\n", config->usart, config->baudrate);
    dbg_flush();
}

int print(char *fmt, va_list args);
void dbg_log(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	print(fmt, args);
	va_end(args);
}



extern void print_and_reset_buffer(void);
void dbg_flush(void)
{
	if(console_putc == NULL){
		panic("Error: console_putc not initialized");
	}
	print_and_reset_buffer();
}

void panic(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	print(fmt, args);
	va_end(args);
	dbg_flush();
	__asm__ volatile ("bkpt\n");
	while (1){};
}
