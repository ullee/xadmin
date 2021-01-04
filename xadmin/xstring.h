#ifndef __LD_STRING_H_INCLUDED__
#define __LD_STRING_H_INCLUDED__

#include <string.h>
#include <sys/types.h>
#include <stdarg.h>

#include "xadmin.h"

struct string {
    uint32_t len;   /* string length */
    uint8_t  *data; /* string data */
};

#define string(_str)   { sizeof(_str) - 1, (uint8_t *)(_str) }
#define null_string    { 0, NULL }

#define string_set_text(_str, _text) do {       \
    (_str)->len = (uint32_t)(sizeof(_text) - 1);\
    (_str)->data = (uint8_t *)(_text);          \
} while (0);

#define string_set_raw(_str, _raw) do {         \
    (_str)->len = (uint32_t)(ld_strlen(_raw));  \
    (_str)->data = (uint8_t *)(_raw);           \
} while (0);

void string_init(struct string *str);
void string_deinit(struct string *str);
bool string_empty(const struct string *str);
int string_duplicate(struct string *dst, const struct string *src);
int string_copy(struct string *dst, const uint8_t *src, uint32_t srclen);
int string_compare(const struct string *s1, const struct string *s2);

/*
 * Wrapper around common routines for manipulating C character
 * strings
 */
#define ld_memcpy(_d, _c, _n)           \
    memcpy(_d, _c, (size_t)(_n))

#define ld_memmove(_d, _c, _n)          \
    memmove(_d, _c, (size_t)(_n))

#define ld_memchr(_d, _c, _n)           \
    memchr(_d, _c, (size_t)(_n))

#define ld_strlen(_s)                   \
    strlen((char *)(_s))

#define ld_strncmp(_s1, _s2, _n)        \
    strncmp((char *)(_s1), (char *)(_s2), (size_t)(_n))

#define ld_strchr(_p, _l, _c)           \
    _ld_strchr((uint8_t *)(_p), (uint8_t *)(_l), (uint8_t)(_c))

#define ld_strrchr(_p, _s, _c)          \
    _ld_strrchr((uint8_t *)(_p),(uint8_t *)(_s), (uint8_t)(_c))

#define ld_strndup(_s, _n)              \
    (uint8_t *)strndup((char *)(_s), (size_t)(_n));

/*
 * snprintf(s, n, ...) will write at most n - 1 of the characters printed into
 * the output string; the nth character then gets the terminating `\0'; if
 * the return value is greater than or equal to the n argument, the string
 * was too short and some of the printed characters were discarded; the output
 * is always null-terminated.
 *
 * Note that, the return value of snprintf() is always the number of characters
 * that would be printed into the output string, assuming n were limited not
 * including the trailing `\0' used to end output.
 *
 * scnprintf(s, n, ...) is same as snprintf() except, it returns the number
 * of characters printed into the output string not including the trailing '\0'
 */
#define ld_snprintf(_s, _n, ...)        \
    snprintf((char *)(_s), (size_t)(_n), __VA_ARGS__)

#define ld_scnprintf(_s, _n, ...)       \
    _scnprintf((char *)(_s), (size_t)(_n), __VA_ARGS__)

#define ld_vsnprintf(_s, _n, _f, _a)    \
    vsnprintf((char *)(_s), (size_t)(_n), _f, _a)

#define ld_vscnprintf(_s, _n, _f, _a)   \
    _vscnprintf((char *)(_s), (size_t)(_n), _f, _a)

#define ld_strftime(_s, _n, fmt, tm)        \
    (int)strftime((char *)(_s), (size_t)(_n), fmt, tm)

/*
 * A (very) limited version of snprintf
 * @param   to   Destination buffer
 * @param   n    Size of destination buffer
 * @param   fmt  printf() style format string
 * @returns Number of bytes written, including terminating '\0'
 * Supports 'd' 'i' 'u' 'x' 'p' 's' conversion
 * Supports 'l' and 'll' modifiers for integral types
 * Does not support any width/precision
 * Implemented with simplicity, and async-signal-safety in mind
 */
int _safe_vsnprintf(char *to, size_t size, const char *format, va_list ap);
int _safe_snprintf(char *to, size_t n, const char *fmt, ...);

#define ld_safe_snprintf(_s, _n, ...)       \
    _safe_snprintf((char *)(_s), (size_t)(_n), __VA_ARGS__)

#define ld_safe_vsnprintf(_s, _n, _f, _a)   \
    _safe_vsnprintf((char *)(_s), (size_t)(_n), _f, _a)

static inline uint8_t *
_ld_strchr(uint8_t *p, uint8_t *last, uint8_t c)
{
    while (p < last) {
        if (*p == c) {
            return p;
        }
        p++;
    }

    return NULL;
}

static inline uint8_t *
_ld_strrchr(uint8_t *p, uint8_t *start, uint8_t c)
{
    while (p >= start) {
        if (*p == c) {
            return p;
        }
        p--;
    }

    return NULL;
}

#endif
