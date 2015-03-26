#ifndef TIP_H
#define TIP_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#define TIP_INT 0
#define TIP_FLOAT 1
#define TIP_STR 2

typedef struct int_col{
    unsigned long numrows;
    long *d;
} int_col;

typedef struct double_col{
    unsigned long numrows;
    double *d;
} double_col;

typedef struct str_col{
    unsigned long numrows;
    unsigned long *offset;
    unsigned char *d;
} str_col;


size_t memcspn(const char *string, size_t strlen, const char *notin, size_t notinlen)
{
    register const char *s1, *s2;
    int i,j;

    for (s1 = string, i = 0; i<strlen; s1++, i++) {
            for(s2 = notin, j = 0; *s2 != *s1 && j < notinlen; s2++, j++)
                    /* EMPTY */ ;
            if (j != notinlen)
                    break;
    }
    return s1 - string;
}


unsigned long ftip(FILE *fp,int numcols,const int type[], void ***cols, unsigned char delim, unsigned char eordelim,unsigned long skiprecs);

unsigned long tip(const char *buff,unsigned long buffsize,int numcols,const int type[], void ***cols, unsigned char delim, unsigned char eordelim,unsigned long skiprecs);

long fsize(FILE *fp);



#endif
