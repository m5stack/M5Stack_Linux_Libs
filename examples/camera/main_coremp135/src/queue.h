/* queue.h */
// Copyright (c) 2024 M5Stack Technology CO LTD
#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <stdint.h>

typedef struct QueueData {
    void* pData;
    uint32_t Length;
} sQueueData;

typedef struct {
    sQueueData Data[CONFIG_QUEUE_SIZE];
    int HeadIndex;
    int TailIndex;
    pthread_mutex_t QueueMutex;
} sQueue;

int QueueInit(sQueue* pQueuePrivateData);
int QueuePutData(sQueueData* pData);
// int QueuePushBack(sQueue* pQueuePrivateData, sQueueData* pData);
int QueuePopData(sQueue* pQueuePrivateData, sQueueData* pData);
int QueueCallback(sQueueData* pQueueData);

#endif
