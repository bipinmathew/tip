#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "tip.h"


int main(int argc, char *argv[]){
    FILE *fp;
    unsigned long i,numrecs;

    int type[9] = {TIP_STR,TIP_INT,TIP_INT,TIP_INT,TIP_INT,TIP_FLOAT,TIP_FLOAT,TIP_INT,TIP_FLOAT};
    // int type[9] = {TIP_STR,TIP_INT,TIP_INT,TIP_INT};

    void **cols;
    if(argc < 2){
        printf("usage: tip <filename>\n");
        exit(1);
    }
    if((fp=fopen(argv[1],"r"))==NULL){
        printf("Error openning %s for reading.\n",argv[1]);
        exit(1);
    }

    numrecs = ftip(fp,9,type,&cols,',','\n',1);

    for(i=0;i<numrecs;i++){
         printf("%d \n",((int*)cols[1])[i]);
    }

    fclose(fp);
    return(0);

}
