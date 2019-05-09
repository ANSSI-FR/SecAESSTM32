#ifndef _TYPES_H
# define _TYPES_H

#include <stdint.h>

# if defined(__CC_ARM)
#  define __ASM            __asm      /* asm keyword for ARM Compiler    */
#  define __INLINE         static __inline   /* inline keyword for ARM Compiler */
# elif defined(__GNUC__)
#  define __ASM            __asm      /* asm keyword for GNU Compiler    */
#  define __INLINE        static inline     /* inline keyword for GNU Compiler */
# endif


#endif
