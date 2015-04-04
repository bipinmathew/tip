#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "tip.h"

int initcols(int numcols, const int type[], void ***cols,int numrows){
    unsigned long t;
    void **mycols;

    mycols = (void *)malloc(numcols*sizeof(void*));
    for(t=0;t<numcols;t++){
        switch(type[t]){
            case TIP_INT:
                mycols[t] = (col_int *)malloc(sizeof(col_int));
            break;
            case TIP_FLOAT:
                mycols[t] = (col_double *)malloc(sizeof(col_double));
            break;
            case TIP_STR:
                mycols[t] = (col_str *)malloc(sizeof(col_str));
                ((col_str*)mycols[t])->_numrows=0;
                ((col_str*)mycols[t])->_numbytes=0;
                ((col_str*)mycols[t])->numrows=0;
                ((col_str*)mycols[t])->numbytes=0;
            break;
        }
    }
    realloccols(numcols,type,&mycols,numrows);
    *cols = mycols;
    return(0);
}


int realloccols(int numcols,const int type[],void ***cols,int numrows){
    unsigned int i,avgstrlen;
    unsigned long _numbytes;
    void **mycols;
    mycols = *cols;
    for(i=0;i<numcols;i++){
        switch(type[i]){
            case TIP_INT:
                ((col_int*)mycols[i])->d = realloc(((col_int*)mycols[i])->d,numrows*sizeof(long));
            break;
            case TIP_FLOAT:
                ((col_double*)mycols[i])->d = realloc(((col_double*)mycols[i])->d,numrows*sizeof(double));
            break;
            case TIP_STR:
                ((col_str*)mycols[i])->offset = realloc(((col_str*)mycols[i])->offset,numrows*sizeof(unsigned long));
                ((col_str*)mycols[i])->_numrows=numrows;
                if(((col_str*)mycols[i])->_numbytes==0)
                    avgstrlen = 128;
                else
                    avgstrlen = ((col_str*)mycols[i])->numbytes / ((col_str*)mycols[i])->numrows;

                _numbytes = numrows*avgstrlen*sizeof(char);
                if(((col_str*)mycols[i])->_numbytes < _numbytes){
                    ((col_str*)mycols[i])->d = realloc(((col_str*)mycols[i])->d,_numbytes);
                    ((col_str*)mycols[i])->_numbytes = _numbytes;
                }
            break;
        }
    }
    *cols = mycols;
    return(0);
}

long fsize(FILE* fp){
    long current,size;
    current = ftell(fp);
    fseek(fp,0,SEEK_END);
    size = ftell(fp);
    fseek(fp,current,SEEK_SET);
    return(size);

}

unsigned long ftip(FILE *fp,int numcols,const int type[], void ***cols, unsigned char delim, unsigned char eordelim,unsigned long skiprecs){
    int fd;
    long f_size;
    char *mm,*data;
    unsigned long numrecs;

    f_size = fsize(fp);
    fd = fileno(fp);

    mm = mmap((caddr_t)0,f_size,PROT_READ,MAP_SHARED,fd,0);
    data = &mm[ftell(fp)];

    numrecs = tip(data,f_size,numcols,type, cols,delim,eordelim,skiprecs);
    if(munmap(mm,f_size)==-1){
        perror("Failed to un-memory map data.");
    }
    return(numrecs);
}

unsigned long tip(const char *buff,unsigned long buffsize,int numcols,const int type[], void ***cols, unsigned char delim, unsigned char eordelim,unsigned long skiprecs){
    unsigned long i,t,*eor,rec=0,offset=0;
    const char *ptr;
    void **mycols;

    mycols = *cols;

    int numrecs = 1024;
    initcols(numcols,type,&mycols,numrecs);
    eor = calloc(numrecs,sizeof(unsigned long));

    for(i=0;i<buffsize;i++){
        if(eordelim==buff[i]){
            eor[rec]=i;
            if(numrecs==++rec){
                numrecs = 2*numrecs;
                eor = realloc(eor,numrecs*sizeof(unsigned long));
                realloccols(numcols,type,&mycols,numrecs);
            }
        }

    }


    for(i=skiprecs;i<rec;i++){
        ptr = &buff[(i<1)?0:(eor[i-1]+1)];
        t=0;
        do{
            switch(type[t]){
                case TIP_INT:
                    ((col_int*)mycols[t])->d[i-skiprecs] = atol(ptr);
                break;
                case TIP_FLOAT:
                    ((col_double*)mycols[t])->d[i-skiprecs] = atof(ptr);
                break;
                case TIP_STR:
                    for(offset=0;offset<1000;offset++)
                        if(ptr[offset]==delim||ptr[offset]==eordelim)
                            break;
                    memcpy(&((col_str*)mycols[t])->d[((col_str*)mycols[t])->numbytes],ptr,offset*sizeof(char));
                    ((col_str*)mycols[t])->numbytes += offset*sizeof(char);
                    if(((col_str*)mycols[t])->numrows == 0){
                        ((col_str*)mycols[t])->offset[0]=offset;
                        ((col_str*)mycols[t])->numrows+= 1;
                    }
                    else{
                        ((col_str*)mycols[t])->offset[((col_str*)mycols[t])->numrows]=((col_str*)mycols[t])->offset[((col_str*)mycols[t])->numrows-1]+offset;
                        ((col_str*)mycols[t])->numrows+= 1;
                    }

                    // cols[i,t] = realloc(cols[i],numrows*sizeof(char*));
                break;
            }
            ptr = (char *)memchr(ptr,delim,&buff[eor[i]]-ptr);
            ptr++;
        }while((t++)<(numcols-1));

    }
    // Lets get some memory back.
    realloccols(numcols,type,&mycols,rec-skiprecs);
    free(eor);
    *cols = mycols;
    return(rec-skiprecs);
}

