#ifndef DEBUG_H_
# define DEBUG_H_

# include "cortex_m4_systick.h"
# include "stm32f4xx_usart.h"


void debug_console_init(usart_config_t *config);

/**
 * dbg_log - log strings in ring buffer
 * @fmt: format string
 */
void dbg_log(char *fmt, ...);
/**
 * dbg_flush - flush the ring buffer to UART
 */
void dbg_flush(void);

/**
 * panic - output string on UART, flush ring buffer and stop
 * @fmt: format string
 */
void panic(char *fmt, ...);

# define assert(EXP)									\
	do {										\
		if (!(EXP))								\
			panic("Assert in file %s on line %d\n", __FILE__, __LINE__);	\
	} while (0)

# if DEBUG_LVL >= 3
#  define LOG(fmt, ...) dbg_log("%lld: [II] %s:%d, %s:"fmt, get_ticks(), __FILE__, __LINE__,  __FUNCTION__, ##__VA_ARGS__)
# else
#  define LOG(fmt, ...) do {} while (0)
# endif

# if DEBUG_LVL >= 2
#  define WARN(fmt, ...) dbg_log("%lld: [WW] %s:%d, %s:"fmt, get_ticks(), __FILE__, __LINE__,  __FUNCTION__, ##__VA_ARGS__)
# else
#  define WARN(fmt, ...) do {} while (0)
# endif

# if DEBUG_LVL >= 1
extern volatile int logging;
#  define ERROR(fmt, ...)							\
	do {									\
		dbg_log("%lld: [EE] %s:%d, %s:"fmt, get_ticks(), __FILE__, __LINE__,  __FUNCTION__, ##__VA_ARGS__);	\
		/*if (logging)*/							\
			dbg_flush();						\
	} while (0)
# else
#  define ERROR(fmt, ...) do {} while (0)
# endif

#define LOG_CL(fmt, ...) dbg_log(""fmt, ##__VA_ARGS__)

void init_ring_buffer(void);

#endif /* ! DEBUG_H_ */
