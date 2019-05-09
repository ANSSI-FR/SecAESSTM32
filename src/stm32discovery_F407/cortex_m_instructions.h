/* This file has been written according to core_cmInstr.h from ARM */
#ifndef CORTEX_M_INSTRUCTIONS_H
# define CORTEX_M_INSTRUCTIONS_H

# if defined(__CC_ARM)

/* No Operation */
#  define __NOP		__nop
/* Instruction Synchronization Barrier */
#  define __ISB()	__isb(0xF)
/* Data Synchronization Barrier */
#  define __DSB()	__dsb(0xF)
/* Data Memory Barrier */
#  define __DMB()	__dmb(0xF)
/* Reverse byte order (32 bit) */
#  define __REV		__rev
/* Reverse byte order (16 bit) */
static __INLINE __ASM uint32_t __REV16(uint32_t value)
{
	rev16 r0, r0
	bx lr
}
/* Breakpoint */
#  define __BKPT	__bkpt

# elif defined(__GNUC__)
#pragma GCC push_options
#pragma GCC optimize("O0")

/* No Operation */
static inline __attribute__((always_inline)) void __NOP(void)
{
	__asm__ volatile ("nop");
}
/* Instruction Synchronization Barrier */
static inline __attribute__((always_inline)) void __ISB(void)
{
	__asm__ volatile("isb");
}
/* Data Synchronization Barrier */
static inline __attribute__((always_inline)) void __DSB(void)
{
	__asm__ volatile("dsb");
}
/* Data Memory Barrier */
static inline __attribute__((always_inline)) void __DMB(void)
{
	__asm__ volatile("dmb");
}
/* Reverse byte order (32 bit) */
static inline __attribute__((always_inline)) uint32_t __REV(uint32_t value)
{
	uint32_t result;

	__asm__ volatile ("rev %0, %1" : "=r" (result) : "r" (value));
	return result;
}
/* Reverse byte order (16 bit) */
static inline __attribute__((always_inline)) uint32_t __REV16(uint32_t value)
{
	uint32_t result;

	__asm__ volatile ("rev16 %0, %1" : "=r" (result) : "r" (value));
	return result;
}
/* Breakpoint */
static inline __attribute__((always_inline)) void __BKPT(void)
{
	__asm__ volatile ("bkpt");
}

#pragma GCC pop_options

# endif

#endif /* CORTEX_M_INSTRUCTIONS_H */
