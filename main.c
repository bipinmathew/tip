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

    // int type[9] = {TIP_STR,TIP_INT,TIP_INT,TIP_INT,TIP_INT,TIP_FLOAT,TIP_FLOAT,TIP_INT,TIP_FLOAT};
    int type[4] = {TIP_STR,TIP_INT,TIP_INT,TIP_INT};
    // int type[2] = {TIP_INT,TIP_INT};

    void **cols;
    if(argc < 2){
        printf("usage: tip <filename>\n");
        exit(1);
    }
    if((fp=fopen(argv[1],"r"))==NULL){
        printf("Error openning %s for reading.\n",argv[1]);
        exit(1);
    }

    numrecs = ftip(fp,4,type,&cols,'\t','\n',0);

    for(i=0;i<numrecs;i++){
         printf("%lu \n",((col_str*)cols[0])->offset[i]);
    }

    fclose(fp);
    return(0);

}
