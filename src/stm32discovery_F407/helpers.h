#ifndef _HELPER_H
# define _HELPER_H
#include "types.h"
#include "cortex_m_functions.h"

# define REG_ADDR(addr)                      ((volatile uint32_t *)(addr))
# define REG_VALUE(reg, value, pos, mask)    ((reg)  |= (((value) << (pos)) & (mask)))

# define SET_BIT(REG, BIT)     ((REG) |= (BIT))
# define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
# define READ_BIT(REG, BIT)    ((REG) & (BIT))
# define CLEAR_REG(REG)        ((REG) = (0x0))

# define ARRAY_SIZE(array, type)	(sizeof(array) / sizeof(type))
# define __packed			__attribute__((__packed__))
#ifndef NULL
#define NULL 0
#endif

/*
 * These macros assume that the coding style (bits_name_Msk and bits_name_Pos)
 * is respected when defining registers bitfields
 */
# define set_reg(REG, VALUE, BITS)	set_reg_value(REG, VALUE, BITS##_Msk, BITS##_Pos)
# define get_reg(REG, BITS)		get_reg_value(REG, BITS##_Msk, BITS##_Pos)

/*
 * Define time to sleep (for loop)
 */
#define MICRO_TIME  	1
#define SHORT_TIME  	3
#define MEDIUM_TIME 	5
#define LONG_TIME   	6
#define DFU_TIME    	24

__INLINE uint32_t  get_reg_value(volatile uint32_t *reg, uint32_t mask, uint8_t pos);
__INLINE uint8_t   set_reg_value(volatile uint32_t *reg, uint32_t value, uint32_t mask, uint8_t pos);

__INLINE uint32_t  read_reg_value(volatile uint32_t *reg);
__INLINE void      write_reg_value(volatile uint32_t *reg, uint32_t value);

__INLINE void set_reg_bits(volatile uint32_t *reg, uint32_t value);
__INLINE void clear_reg_bits(volatile uint32_t *reg, uint32_t value);

__INLINE uint32_t to_big32(uint32_t value);
__INLINE uint16_t to_big16(uint16_t value);
__INLINE uint32_t to_little32(uint32_t value);
__INLINE uint16_t to_little16(uint16_t value);
__INLINE uint32_t from_big32(uint32_t value);
__INLINE uint16_t from_big16(uint16_t value);
__INLINE uint32_t from_little32(uint32_t value);
__INLINE uint16_t from_little16(uint16_t value);

void sleep_intern(uint8_t length);

__INLINE uint32_t get_reg_value(volatile uint32_t *reg, uint32_t mask, uint8_t pos)
{
	if ((mask == 0x00) || (pos > 31))
		return 0;

	return (uint32_t)(((*reg) & mask) >> pos);
}

__INLINE uint8_t set_reg_value(volatile uint32_t *reg, uint32_t value, uint32_t mask, uint8_t pos)
{
	uint32_t tmp;

	if (pos > 31)
		return -1;

	if (mask == 0xFFFFFFFF) {
		(*reg) = value;
	} else {
		tmp = read_reg_value(reg);
		tmp &= ~mask;
		tmp |= (value << pos) & mask;
		write_reg_value(reg, tmp);
	}

	return 0;
}

__INLINE uint32_t read_reg_value(volatile uint32_t *reg)
{
	return get_reg_value(reg, 0xFFFFFFFF, 0);
}

__INLINE void write_reg_value(volatile uint32_t *reg, uint32_t value)
{
	set_reg_value(reg, value, 0xFFFFFFFF, 0);
}

__INLINE void set_reg_bits(volatile uint32_t *reg, uint32_t value)
{
	*reg |= value;
}

__INLINE void clear_reg_bits(volatile uint32_t *reg, uint32_t value)
{
	*reg &= (uint32_t)~(value);
}

__INLINE uint32_t to_big32(uint32_t value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return ((value & 0xff) << 24) | ((value & 0xff00) << 8)
		| ((value & 0xff0000) >> 8) | ((value & 0xff000000) >> 24);
#else
	return value;
#endif
}

__INLINE uint16_t to_big16(uint16_t value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return ((value & 0xff) << 8) | ((value & 0xff00) >> 8);
#else
	return value;
#endif
}

__INLINE uint32_t to_little32(uint32_t value)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return ((value & 0xff) << 24) | ((value & 0xff00) << 8)
		| ((value & 0xff0000) >> 8) | ((value & 0xff000000) >> 24);
#else
	return value;
#endif
}

__INLINE uint16_t to_little16(uint16_t value)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return ((value & 0xff) << 8) | ((value & 0xff00) >> 8);
#else
	return value;
#endif
}

__INLINE uint32_t from_big32(uint32_t value)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return value;
#else
	return ((value & 0xff) << 24) | ((value & 0xff00) << 8)
		| ((value & 0xff0000) >> 8) | ((value & 0xff000000) >> 24);
#endif
}
__INLINE uint16_t from_big16(uint16_t value)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return value;
#else
	return ((value & 0xff) << 8) | ((value & 0xff00) >> 8);
#endif
}
__INLINE uint32_t from_little32(uint32_t value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return value;
#else
	return ((value & 0xff) << 24) | ((value & 0xff00) << 8)
		| ((value & 0xff0000) >> 8) | ((value & 0xff000000) >> 24);
#endif
}
__INLINE uint16_t from_little16(uint16_t value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return value;
#else
	return ((value & 0xff) << 8) | ((value & 0xff00) >> 8);
#endif
}


#endif /* _HELPER_H */
