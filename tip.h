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
    unsigned char *d;
} str_col;



unsigned long ftip(FILE *fp,int numcols,const int type[], void ***cols, unsigned char delim, unsigned char eordelim,unsigned long skiprecs);

unsigned long tip(const char *buff,unsigned long buffsize,int numcols,const int type[], void ***cols, unsigned char delim, unsigned char eordelim,unsigned long skiprecs);

off_t fsize(int fd);



#endif
