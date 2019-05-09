#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <stdarg.h>

#ifdef USE_KEIL_PLATFORM

#define USE_FAKE_PRINTF

#else

int print(char *fmt, va_list args);

int printf(char *fmt, ...);

int sprintf(char *dst, char *fmt, ...);

int snprintf(char *dst, int len, char *fmt, ...);

void print_and_reset_buffer(void);

#endif

#endif
