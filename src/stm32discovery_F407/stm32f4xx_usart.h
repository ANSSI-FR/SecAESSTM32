#ifndef STM32F4XX_USART_H
# define STM32F4XX_USART_H

# include "helpers.h"
#include "stm32f4xx_usart_regs.h"

typedef void (*cb_usart_irq_handler_t)(void);
typedef uint16_t (*cb_usart_getc_t)(void);
typedef void (*cb_usart_putc_t)(uint16_t);

/* Defines the USART mode that we consider 
 * [RB] TODO: to be completed with other USART modes if/when needed! (IrDA, LIN, ...)
 * For now, only the classical UART console mode and the SMARTCARD modes are
 * implemented.
 */
typedef enum { UART, SMARTCARD, CUSTOM } usart_mode;

/* [RB] TODO: add some magic initialization values and assert to avoid manipulation 
 * of uninitialized structures!
 */
#define USART_SET_BAUDRATE           ((uint32_t)1 << 1)
#define USART_SET_WORD_LENGTH        ((uint32_t)1 << 2)
#define USART_SET_PARITY             ((uint32_t)1 << 3)
#define USART_SET_STOP_BITS          ((uint32_t)1 << 4)
#define USART_SET_HW_FLOW_CTRL       ((uint32_t)1 << 5)
#define USART_SET_OPTIONS_CR1        ((uint32_t)1 << 6)
#define USART_SET_OPTIONS_CR2        ((uint32_t)1 << 7)
#define USART_SET_GUARD_TIME_PS      ((uint32_t)1 << 8)
#define USART_SET_CB_RCV_IRQ         ((uint32_t)1 << 9)
#define USART_SET_ALL	 	     (USART_SET_BAUDRATE | USART_SET_WORD_LENGTH | USART_SET_PARITY | USART_SET_STOP_BITS | USART_SET_HW_FLOW_CTRL |\
				      USART_SET_OPTIONS_CR1 | USART_SET_OPTIONS_CR2 | USART_SET_GUARD_TIME_PS | USART_SET_CB_RCV_IRQ)

typedef struct __packed {
	/* The things we want to set as a mask */
        uint32_t set_mask;
	uint8_t usart;
	usart_mode mode;
	uint32_t baudrate;      /* USART_BRR:DIV_Mantissa[11:0]: and DIV_Fraction[3:0]: */
	uint32_t word_length;    /* USART_CR1:Bit 12 M: Word length */
	uint32_t parity;         /* USART_CR1:Bit 9 PS: Parity selection and Bit 10 PCE: Parity control enable */
	uint32_t stop_bits;      /* USART_CR2:Bits 13:12 STOP: STOP bits,  */
	uint32_t hw_flow_control; /* USART_CR3:
                                    Bit 11 ONEBIT: One sample bit method enable
                                    Bit 10 CTSIE: CTS interrupt enable
                                    Bit 9 CTSE: CTS enable
                                    Bit 8 RTSE: RTS enable
                                    Bit 7 DMAT: DMA enable transmitter
                                    Bit 6 DMAR: DMA enable receiver
                                    Bit 5 SCEN: Smartcard mode enable
                                    Bit 4 NACK: Smartcard NACK enable
                                    Bit 3 HDSEL: Half-duplex selection
                                    Bit 2 IRLP: IrDA low-power
                                    Bit 1 IREN: IrDA mode enable
                                    Bit 0 EIE: Error interrupt enable */
	/* Additional options in CR1 */
	uint32_t options_cr1;    /* USART_CR1:
                                    Bit 15 OVER8:
                                    Bit 11 WAKE: Wakeup method
                                    Bit 8 PEIE: PE interrupt enable
                                    Bit 7 TXEIE: TXE interrupt enable
                                    Bit 6 TCIE: Transmission complete interrupt enable
                                    Bit 5 RXNEIE: RXNE interrupt enable
                                    Bit 4 IDLEIE: IDLE interrupt enable
                                    Bit 3 TE: Transmitter enable
                                    Bit 2 RE: Receiver enable
                                    Bit 1 RWU: Receiver wakeup
                                    Bit 0 SBK: Send break */
	/* Additional options in CR2 */
	uint32_t options_cr2;/* USART_CR2:
				Bit 14 LINEN: LIN mode enable
				Bit 11 CLKEN: Clock enable
				Bit 10 CPOL: Clock polarity
				Bit 9 CPHA: Clock phase
				Bit 8 LBCL: Last bit clock pulse
				Bit 6 LBDIE: LIN break detection interrupt enable
				Bit 5 LBDL: lin break detection length */
	uint32_t guard_time_prescaler;   /* USART_GTPR: (used for Smartcard and IrDA modes
					  * Bits 15:8 GT[7:0]: Guard time value
					  * Bits 7:0 PSC[7:0]: Prescaler value */
	cb_usart_irq_handler_t callback_irq_handler;
	cb_usart_getc_t *callback_usart_getc_ptr;
	cb_usart_putc_t *callback_usart_putc_ptr;
} usart_config_t;

#define USART_CONFIG_WORD_LENGTH_BITS_Msk (USART_CR1_M_Msk)
#define USART_CONFIG_WORD_LENGTH_BITS_Pos 0

#define USART_CONFIG_PARITY_Msk (USART_CR1_PS_Msk | USART_CR1_PCE_Msk)
#define USART_CONFIG_PARITY_Pos 0

#define USART_CONFIG_STOP_BITS_Msk (USART_CR2_STOP_Msk)
#define USART_CONFIG_STOP_BITS_Pos 0

#define USART_CONFIG_OPTIONS_CR1_Msk (USART_CR1_OVER8_Msk \
                                | USART_CR1_WAKE_Msk \
                                | USART_CR1_PEIE_Msk \
                                | USART_CR1_TXEIE_Msk \
                                | USART_CR1_TCIE_Msk \
                                | USART_CR1_RXNEIE_Msk \
                                | USART_CR1_IDLEIE_Msk \
                                | USART_CR1_TE_Msk \
                                | USART_CR1_RE_Msk \
                                | USART_CR1_RWU_Msk \
                                | USART_CR1_SBK_Msk )
#define USART_CONFIG_OPTIONS_CR1_Pos 0

#define USART_CONFIG_OPTIONS_CR2_Msk ( USART_CR2_LINEN_Msk \
				| USART_CR2_CLKEN_Msk \
				| USART_CR2_CPOL_Msk \
				| USART_CR2_CPHA_Msk \
				| USART_CR2_LBCL_Msk \
				| USART_CR2_LBDIE_Msk \
				| USART_CR2_LBDL_Msk )
#define USART_CONFIG_OPTIONS_CR2_Pos 0

#define USART_CONFIG_HW_FLW_CTRL_Msk (USART_CR3_ONEBIT_Msk \
                                    | USART_CR3_CTSIE_Msk \
                                    | USART_CR3_CTSE_Msk \
                                    | USART_CR3_RTSE_Msk \
                                    | USART_CR3_DMAT_Msk \
                                    | USART_CR3_DMAR_Msk \
                                    | USART_CR3_SCEN_Msk \
                                    | USART_CR3_NACK_Msk \
                                    | USART_CR3_HDSEL_Msk \
                                    | USART_CR3_IRLP_Msk \
                                    | USART_CR3_IREN_Msk \
                                    | USART_CR3_EIE_Msk )
#define USART_CONFIG_HW_FLW_CTRL_Pos    0



#define USART_CONFIG_GUARD_TIME_PRESCALER_Msk (USART_GTPR_GT_Msk | USART_GTPR_PSC_Msk)
#define USART_CONFIG_GUARD_TIME_PRESCALER_Pos	0

/**
 * usart_init - Initialize the USART
 * @n: USART to use (1 or 4).
 * @callback_data_received: function called when data is received on the RX
 * pin. If this argument is NULL, USART IRQs are disabled.
 *
 * This function initialize USART1 (n = 1) or UART4 (n = 4) as 8N1 at a
 * baudrate of 115200
 */
void usart_init(usart_config_t *config);

/*
 * usart_enable - Ensable the specific USART
 */
void usart_enable(usart_config_t *config);


/*
 * usart_disable - Disable the specific USART
 */
void usart_disable(usart_config_t *config);


/**
 * usart_putc - Send a character
 * @c: Character to send.
 */
void usart_putc(uint8_t usart, uint16_t c);


/**
 * usart_write - Send a string
 * @msg: string of size @len to send.
 * @len: size of @msg.
 */
void usart_write(uint8_t usart, char *msg, uint32_t len);

/**
 * usart_getc - Read a character
 * Return: The character read.
 */
uint16_t usart_getc(uint8_t usart);


/**
 * usart_read - Read a string
 * @buf: Address of the buffer in which the read characters will be written.
 * The size of the buffer must be at least @len.
 * @len: Number of characters to read.
 */
uint32_t usart_read(uint8_t usart, char *buf, uint32_t len);

/**
 * usart_disable_dma - Enable the DMA for USART
 *
 * Disable for transmit only.
 * See also usart_enable_dma.
 */
void usart_disable_dma(uint8_t usart);

/**
 * usart_enable_dma - Enable the DMA for USART
 *
 * Enable for transmit only.
 * See also usart_disable_dma.
 */
void usart_enable_dma(uint8_t usart);

/**
 * usart_get_data_addr - Getter for the USART's data register address
 * Return: The address of the USART's data register where the data should be
 * written.
 */
volatile uint32_t *usart_get_data_addr(uint8_t usart);

/* Get the clock frequency value of the APB bus driving the USART */
uint32_t usart_get_bus_clock(usart_config_t *config);

#endif /* STM32F4XX_USART_H */
