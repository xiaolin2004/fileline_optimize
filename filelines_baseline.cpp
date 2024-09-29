#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h> // 添加 pthread 库
#include <queue>     // 添加队列库
#include <mutex>     // 添加互斥锁库
#include <condition_variable> // 添加条件变量库
#include <immintrin.h>
#define MAX_LEN 1024
#define BLOCK_SIZE 1024
std::queue<char*> buffer; // 用于存储读取的块
std::mutex mtx;           // 互斥锁
std::condition_variable cv; // 条件变量
bool done = false;        // 标志生产者是否完成

// 生产者线程函数
void* producer(void* arg) {
    char *filepath = (char*)arg;
    int handle = open(filepath, O_RDONLY);
    if (handle < 0) return nullptr;

    char *bp = (char *)malloc(BLOCK_SIZE);
    while (true) {
        int bytes_read = read(handle, bp, BLOCK_SIZE);
        if (bytes_read <= 0) break;

        std::unique_lock<std::mutex> lock(mtx);
        buffer.push(bp); // 将读取的块放入队列
        cv.notify_one(); // 通知消费者
        bp = (char *)malloc(BLOCK_SIZE); // 分配新的块
    }
    close(handle);
    done = true; // 设置完成标志
    cv.notify_all(); // 通知所有消费者
    return nullptr;
}

// 消费者线程函数
void* consumer(void* arg) {
    uint32_t *total_line_num = (uint32_t*)arg;
    uint32_t *line_num = (uint32_t*)arg + 1; // 假设 line_num 紧跟在 total_line_num 后
    int cur_len = 0;
    __m128i newline_char = _mm_set1_epi8('\n');

    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !buffer.empty() || done; }); // 等待条件

        if (buffer.empty() && done) break; // 如果队列为空且生产者完成，退出

        char *bp = buffer.front();
        buffer.pop();
        lock.unlock(); // 解锁以允许其他线程访问

        // 处理读取的块
        int bytes_read = BLOCK_SIZE; // 假设每次读取的大小
        for (int i = 0; i < bytes_read; i += 16) {
            __m128i chunk = _mm_loadu_si128((__m128i*)(bp + i));
            __m128i cmp = _mm_cmpeq_epi8(chunk, newline_char);
            unsigned int mask = _mm_movemask_epi8(cmp);

            while (mask) {
                int pos = __builtin_ctz(mask);
                (*total_line_num)++;
                
                if (cur_len >= MAX_LEN) line_num[MAX_LEN - 1]++;
                else line_num[cur_len]++;

                cur_len = 0;
                mask &= mask - 1;
                cur_len += pos + 1;
            }
            cur_len += 16 - __builtin_popcount(mask);
        }
        free(bp); // 释放读取的块
    }
    return nullptr;
}

void filelines_baseline(char *filepath, uint32_t *total_line_num, uint32_t *line_num) {
    pthread_t prod, cons;
    pthread_create(&prod, nullptr, producer, filepath);
    pthread_create(&cons, nullptr, consumer, total_line_num);
    pthread_join(prod, nullptr);
    pthread_join(cons, nullptr);
}