#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "filelines_baseline.h"
#include "find_most_freq.h"
#include <immintrin.h>

#define BLOCK_SIZE 1024
void filelines_baseline(char *filepath,uint32_t *total_line_num,uint32_t *line_num){
    int handle;
    if((handle=open(filepath,O_RDONLY))<0) return;
    char *bp=(char *)malloc(BLOCK_SIZE);
    if(bp==NULL){
        close(handle);
        return;
    }
    int cur_len=0;
    __m128i newline_char = _mm_set1_epi8('\n');
    __m128i zero = _mm_setzero_si128();

    while(1){
        int bytes_read=read(handle,bp,BLOCK_SIZE);
        if(bytes_read<=0) break;

        for(int i=0;i<bytes_read;i+=16){
            __m128i chunk = _mm_loadu_si128((__m128i*)(bp + i));
            __m128i cmp = _mm_cmpeq_epi8(chunk, newline_char);
            unsigned int mask = _mm_movemask_epi8(cmp);

            while(mask){
                int pos = __builtin_ctz(mask);
                (*total_line_num)++;
                
                if(cur_len >= MAX_LEN) line_num[MAX_LEN-1]++;
                else line_num[cur_len]++;

                cur_len = 0;
                mask &= mask - 1;
                cur_len += pos + 1;
            }
            cur_len += 16 - __builtin_popcount(mask);
        }
    }
    close(handle);
}