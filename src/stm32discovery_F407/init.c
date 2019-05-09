#include "helpers.h"
#include "stm32f4xx_flash.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_scb.h"
#include "debug.h"
#include "cortex_m_instructions.h"
#include "product.h"
#include "init.h"

/*
 *
 * Configures the System clock source, PLL Multiplier and Divider factors,
 * AHB/APBx prescalers and Flash settings
 * This function should be called only once the RCC clock configuration
 * is reset to the default reset state (done in SystemInit() function).
 *
 */

void SetSysClock(void)
{
	uint32_t StartUpCounter = 0, status = 0;
    /******************************************************************************/
	/*            PLL (clocked by HSE/HSI) used as System clock source                */
	/******************************************************************************/
#ifdef ENABLE_HSE
	/* Enable HSE */
	set_reg_bits(r_CORTEX_M_RCC_CR, RCC_CR_HSEON);

	/* Wait till HSE is ready and if Time out is reached exit */
	do {
		status = read_reg_value(r_CORTEX_M_RCC_CR) & RCC_CR_HSERDY;
		StartUpCounter++;
	} while ((status == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));
#else
    /* Enable HSI */
	set_reg_bits(r_CORTEX_M_RCC_CR, RCC_CR_HSION);

	/* Wait till HSE is ready and if Time out is reached exit */
	do {
		status = read_reg_value(r_CORTEX_M_RCC_CR) & RCC_CR_HSIRDY;
		StartUpCounter++;
	} while ((status == 0) && (StartUpCounter != HSI_STARTUP_TIMEOUT));
#endif
	if (status != RESET)
		status = (uint32_t)0x01;
	else
		status = (uint32_t)0x00;

	if (status == (uint32_t)0x01) {
		/* Enable high performance mode, System frequency up to 168 MHz */
		set_reg_bits(r_CORTEX_M_RCC_APB1ENR, RCC_APB1ENR_PWREN);
		set_reg_bits(r_CORTEX_M_PWR_CR, PWR_CR_VOS_Msk);

		/* HCLK = SYSCLK / 1*/
		set_reg_bits(r_CORTEX_M_RCC_CFGR, RCC_CFGR_HPRE_DIV1);

		/* PCLK2 = HCLK / 2*/
		set_reg_bits(r_CORTEX_M_RCC_CFGR, RCC_CFGR_HPRE2_DIV2);

		/* PCLK1 = HCLK / 4*/
		set_reg_bits(r_CORTEX_M_RCC_CFGR, RCC_CFGR_HPRE1_DIV4);
#ifdef PROD_ENABLE_HSE
		/* Configure the main PLL */
		write_reg_value(r_CORTEX_M_RCC_PLLCFGR, PROD_PLL_M | (PROD_PLL_N << 6) | (((PROD_PLL_P >> 1) - 1) << 16) |
				(RCC_PLLCFGR_PLLSRC_HSE) | (PROD_PLL_Q << 24));
#else
	        write_reg_value(r_CORTEX_M_RCC_PLLCFGR, PROD_PLL_M | (PROD_PLL_N << 6) | (((PROD_PLL_P >> 1) - 1) << 16) |
				(RCC_PLLCFGR_PLLSRC_HSI) | (PROD_PLL_Q << 24));
#endif
		/* Enable the main PLL */
		set_reg_bits(r_CORTEX_M_RCC_CR, RCC_CR_PLLON);

		/* Wait till the main PLL is ready */
		while ((read_reg_value(r_CORTEX_M_RCC_CR) & RCC_CR_PLLRDY) == 0)
			continue;

		/* Configure Flash prefetch, Instruction cache, Data cache and wait state */
		write_reg_value(r_CORTEX_M_FLASH_ACR, FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS);

		/* Select the main PLL as system clock source */
		clear_reg_bits(r_CORTEX_M_RCC_CFGR, RCC_CFGR_SW);
		set_reg_bits(r_CORTEX_M_RCC_CFGR, RCC_CFGR_SW_PLL);

		/* Wait till the main PLL is used as system clock source */
		while ((read_reg_value(r_CORTEX_M_RCC_CFGR) & (uint32_t)RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
			continue;

	} else {
		/* If HSE/I fails to start-up, the application will have wrong
		 * clock configuration. User can add here some code to deal
		 * with this error.
		 */
	}
}


/*
 * void set_vtor(uint32_t)
 * Configure the Vector Table location and offset address
 *
 * WARNING : No interrupts here => IRQs disabled
 *				=> No LOGs here
 */
void set_vtor(uint32_t addr)
{

    __DMB();        /* Data Memory Barrier */
	write_reg_value(r_CORTEX_M_SCB_VTOR, addr);
    __DSB();        /*
                     * Data Synchronization Barrier to ensure all
                     * subsequent instructions use the new configuration
                     */
}


/* void system_init(void)
 * Setup the microcontroller system
 * Initialize the Embedded Flash Interface, the PLL and update the
 * SystemFrequency variable.
 */
void system_init(uint32_t addr)
{
    /* Reset the RCC clock configuration to the default reset state */
	/* Set HSION bit */
	set_reg_bits(r_CORTEX_M_RCC_CR, RCC_CR_HSION);

	/* Reset CFGR register */
	write_reg_value(r_CORTEX_M_RCC_CFGR, 0x00000000);

	/* Reset HSEON, CSSON and PLLON bits */
	//clear_reg_bits(r_CORTEX_M_RCC_CR, RCC_CR_HSEON | RCC_CR_CSSON | RCC_CR_PLLON);
	clear_reg_bits(r_CORTEX_M_RCC_CR, RCC_CR_HSEON | RCC_CR_CSSON | RCC_CR_PLLON);


	/* Reset PLLCFGR register */
	write_reg_value(r_CORTEX_M_RCC_PLLCFGR, 0x24003010);

	/* Reset HSEBYP bit */
	clear_reg_bits(r_CORTEX_M_RCC_CR, RCC_CR_HSEBYP);



	/*
	 * Configure the System clock source, PLL Multiplier and Divider factors,
	 * AHB/APBx prescalers and Flash settings
	 */
	SetSysClock();

	//set_vtor(FLASH_BASE|VECT_TAB_OFFSET);
	set_vtor(addr);
	/* Reset all interrupts */
	write_reg_value(r_CORTEX_M_RCC_CIR, 0x00000000);
}
