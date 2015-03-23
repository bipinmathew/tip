#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "tip.h"


int realloccols(int numcols,const int type[],void **cols,int numrows){
    int i;
    for(i=0;i<numcols;i++){
        switch(type[i]){
            case TIP_INT:
                ((int_col*)cols[i])->d = realloc(((int_col*)cols[i])->d,numrows*sizeof(long));
            break;
            case TIP_FLOAT:
                ((double_col*)cols[i])->d = realloc(((double_col*)cols[i])->d,numrows*sizeof(double));
            break;
            case TIP_STR:
                cols[i] = realloc(cols[i],numrows*sizeof(char*));
            break;
        }
    }
    return(0);
}

int initcols(int numcols, const int type[], void ***cols,int numrows){
    unsigned long t;
    *cols = NULL;
    *cols = (void **)malloc(numcols*sizeof(void*));
    for(t=0;t<numcols;t++){
        switch(type[t]){
            case TIP_INT:
                (*cols)[t] = (int_col **)malloc(sizeof(int_col *));
            break;
            case TIP_FLOAT:
                (*cols)[t] = (double_col **)malloc(sizeof(double_col *));
            break;
            case TIP_STR:
                // cols[i,t] = realloc(cols[i],numrows*sizeof(char*));
            break;
        }
    }
    realloccols(numcols,type,*cols,numrows);
    return(0);
}

off_t fsize(int fd){
    off_t current,size;
    current = lseek(fd,0,SEEK_CUR);
    size = lseek(fd,0,SEEK_END);
    lseek(fd,current,SEEK_SET);
    return(size);

}

unsigned long ftip(FILE *fp,int numcols,const int type[], void ***cols, unsigned char delim, unsigned char eordelim,unsigned long skiprecs){
    int fd;
    off_t f_size;
    char *data;
    unsigned long numrecs;

    fd = fileno(fp);
    f_size = fsize(fd);
    data = mmap((caddr_t)0,f_size,PROT_READ,MAP_SHARED,fd,0);
    numrecs = tip(data,f_size,numcols,type, cols,delim,eordelim,skiprecs);
    if(munmap(data,f_size)==-1){
        perror("Failed to un-memory map data.");
    }
    return(numrecs);
}

unsigned long tip(const char *buff,unsigned long buffsize,int numcols,const int type[], void ***cols, unsigned char delim, unsigned char eordelim,unsigned long skiprecs){
    unsigned long i,t,*eor,rec=0;
    const char *ptr;
    void **mycols;

    int numrecs = 1024;
    initcols(numcols,type,cols,numrecs);
    eor = calloc(numrecs,sizeof(unsigned long));

    for(i=0;i<buffsize;i++){
        if(eordelim==buff[i]){
            eor[rec]=i;
            if(numrecs==++rec){
                numrecs = 2*numrecs;
                eor = realloc(eor,numrecs*sizeof(unsigned long));
                realloccols(numcols,type,*cols,numrecs);
            }
        }

    }

    mycols = *cols;

    for(i=skiprecs;i<rec;i++){
        ptr = &buff[0?0:(eor[i-1]+1)];
        t=0;
        do{
            switch(type[t]){
                case TIP_INT:
                    ((int_col*)mycols[t])->d[i-skiprecs] = atol(ptr);
                break;
                case TIP_FLOAT:
                    ((double_col*)mycols[t])->d[i-skiprecs] = atof(ptr);
                break;
                case TIP_STR:
                    // cols[i,t] = realloc(cols[i],numrows*sizeof(char*));
                break;
            }
            ptr = (char *)memchr(ptr,delim,&buff[eor[i]]-ptr);
            ptr++;
            //printf("Found at location %c \r\n",*ptr);
        }while((t++)<(numcols-1));

    }
    // Lets get some memory back.
    realloccols(numcols,type,*cols,rec-skiprecs);
    free(eor);
    return(rec-skiprecs);
}

