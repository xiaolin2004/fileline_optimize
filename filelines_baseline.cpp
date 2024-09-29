#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "filelines_baseline.h"
#include "find_most_freq.h"

#define BLOCK_SIZE 64
void filelines_baseline(char *filepath,uint32_t *total_line_num,uint32_t *line_num){
    int handle;
    if((handle=open(filepath,O_RDONLY))<0) return;
    char *bp=(char *)malloc(BLOCK_SIZE);
    if(bp==NULL){
        close(handle);
        return;
    }
    int cur_len=0;
    while(1){
        int bytes_read=read(handle,bp,BLOCK_SIZE);
        if(bytes_read<=0) break;
        for(int i=0;i<bytes_read;i++){
            if(bp[i]=='\n'){
                (*total_line_num)++;
                if(cur_len>=MAX_LEN) line_num[MAX_LEN-1]++;
                else line_num[cur_len]++;
                cur_len=0;    
            }
            else{
                cur_len++;
            }

        }
    }
    close(handle);
}