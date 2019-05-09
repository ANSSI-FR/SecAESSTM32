#ifndef __STRING_H__
#define __STRING_H__

#define NULL 0

/* Override the builtins ... */
static inline void local_memcpy(void *dst, const void *src, unsigned int n)
{
        const unsigned char *lsrc = (const unsigned char*)src;
        unsigned char *ldst = (unsigned char*)dst;
        unsigned int i;

        for (i = 0; i < n; i++) {
                *ldst = *lsrc;
                ldst++;
                lsrc++;
        }
}

/* This function is a simple (non-optimized) reimplementation of memset() */
static inline void local_memset(void *v, unsigned char c, unsigned int n)
{
        volatile unsigned char *p = (volatile unsigned char*)v;
        unsigned int i;

        for (i = 0; i < n; i++) {
                *p = c;
                p++;
        }
}

static inline int local_memcmp(const void *s1, const void *s2, int n)
{
    unsigned char u1, u2;
    const unsigned char *t_s1 = (const unsigned char *)s1;
    const unsigned char *t_s2 = (const unsigned char *)s2;
    for (; n--; t_s1++, t_s2++) {
        u1 = *t_s1;
        u2 = *t_s2;
        if (u1 != u2) {
            return (u1 - u2);
        }
    }

    return 0;
}

static inline unsigned int local_strlen(const char *s)
{
    unsigned int i = 0;
    while (*s) {
        i++;
        s++;
    }
    return i;
}

static inline char *local_strncpy(char *dest, const char *src, unsigned int n)
{
        char *return_value = dest;

        while (n && *src) {
                *dest = *src;
                dest++;
                src++;
                n--;
        }

        while (n) {
                *dest = 0;
                dest++;
                n--;
        }

        return return_value;
}

#endif
