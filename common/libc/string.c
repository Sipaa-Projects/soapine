#include <string.h>

int mbtowc (wchar_t * __pwc, const char *s, size_t n)
{
    wchar_t arg;
    int ret = 1;
    if(!s || !*s) return 0;
    arg = (wchar_t)*s;
    if((*s & 128) != 0) {
        if((*s & 32) == 0 && n > 0) { arg = ((*s & 0x1F)<<6)|(*(s+1) & 0x3F); ret = 2; } else
        if((*s & 16) == 0 && n > 1) { arg = ((*s & 0xF)<<12)|((*(s+1) & 0x3F)<<6)|(*(s+2) & 0x3F); ret = 3; } else
        if((*s & 8) == 0 && n > 2) { arg = ((*s & 0x7)<<18)|((*(s+1) & 0x3F)<<12)|((*(s+2) & 0x3F)<<6)|(*(s+3) & 0x3F); ret = 4; }
        else return -1;
    }
    if(__pwc) *__pwc = arg;
    return ret;
}

int wctomb (char *s, wchar_t u)
{
    int ret = 0;
    if(u<0x80) {
        *s = u;
        ret = 1;
    } else if(u<0x800) {
        *(s+0)=((u>>6)&0x1F)|0xC0;
        *(s+1)=(u&0x3F)|0x80;
        ret = 2;
    } else {
        *(s+0)=((u>>12)&0x0F)|0xE0;
        *(s+1)=((u>>6)&0x3F)|0x80;
        *(s+2)=(u&0x3F)|0x80;
        ret = 3;
    }
    return ret;
}

size_t mbstowcs (wchar_t *__pwcs, const char *__s, size_t __n)
{
    int r;
    wchar_t *orig = __pwcs;
    if(!__s || !*__s) return 0;
    while(*__s) {
        r = mbtowc(__pwcs, __s, __n - (size_t)(__pwcs - orig));
        if(r < 0) return (size_t)-1;
        __pwcs++;
        __s += r;
    }
    *__pwcs = 0;
    return (size_t)(__pwcs - orig);
}

size_t wcstombs (char *__s, const wchar_t *__pwcs, size_t __n)
{
    int r;
    char *orig = __s;
    if(!__s || !__pwcs || !*__pwcs) return 0;
    while(*__pwcs && ((size_t)(__s - orig + 4) < __n)) {
        r = wctomb(__s, *__pwcs);
        if(r < 0) return (size_t)-1;
        __pwcs++;
        __s += r;
    }
    *__s = 0;
    return (size_t)(__s - orig);
}

int strlen(char *str)
{
    int i = 0;
    while (str[i] != '\0') i++;
    return i;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n > 0 && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }

    if (n == 0) {
        return 0;
    }

    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

char *strncpy(char *dest, const char *src, size_t n) {
    size_t i;

    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }

    for ( ; i < n; i++) {
        dest[i] = '\0';
    }

    return dest;
}