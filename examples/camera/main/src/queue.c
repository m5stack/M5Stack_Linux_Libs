/* queue.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "queue.h"
 
typedef struct LinkListNode
{
    struct LinkListNode* pNext;
    uint8_t Times;
    void* pData;
} sLinkListNode;
 
static struct {
    int Cnt;
    sQueue* QueueList[CONFIG_QUEUE_LIMIT];
    pthread_mutex_t QueueMutex;
    pthread_mutex_t LinkListMutex;
    sLinkListNode* pLinkListRoot;
    
} sQueuePrivateData;
 
int LinkedListAdd(void* pData)
{
    sLinkListNode* pTempNode = sQueuePrivateData.pLinkListRoot;
 
    if(pTempNode == NULL) {
        // printf("Debug:LinkList root empty, inited \n");
        sQueuePrivateData.pLinkListRoot = malloc(sizeof(sLinkListNode));
        sQueuePrivateData.pLinkListRoot->pNext = NULL;
        sQueuePrivateData.pLinkListRoot->pData = pData;
        sQueuePrivateData.pLinkListRoot->Times = 0;
        return 0;
    }
 
    while(pTempNode->pNext != NULL) {
        pTempNode = pTempNode->pNext;
    }
 
    pTempNode->pNext = malloc(sizeof(sLinkListNode));
    pTempNode->pNext->pNext = NULL;
    pTempNode->pNext->pData = pData;
    pTempNode->pNext->Times = 0;
 
    return 0;
}
 
int LinkedListDel(void* pData)
{
    sLinkListNode* pTempNode = NULL;
    sLinkListNode** ppPre = &sQueuePrivateData.pLinkListRoot;
 
    if(*ppPre == NULL) {
        // printf("Error: LinkList empty\n");
        return -1;
    }
 
    while(*ppPre != NULL) {
        if((*ppPre)->pData == pData) {
            if((*ppPre)->Times == CONFIG_QUEUE_LIMIT - 1) {
                pTempNode = (*ppPre)->pNext;
                free(pData);
                free(*ppPre);
                *ppPre = pTempNode;
                // printf("Debug: free buffer\n");
                break;
            } else {
                // printf("Debug: times not equ limit: %d times\n", (*ppPre)->Times);
                (*ppPre)->Times++;
                break;
            }
        } else {
            ppPre = &(*ppPre)->pNext;
            // printf("Debug: ppPre: %p\n", *ppPre);
            // printf("Debug: next\n");
        }
    }
 
    return 0;
}
 
int BaseQueueInit(void)
{
    sQueuePrivateData.Cnt = 0;
    for(int i = 0; i < CONFIG_QUEUE_LIMIT; i++) {
        sQueuePrivateData.QueueList[i] = NULL;
    }
    sLinkListNode* pTempRoot = sQueuePrivateData.pLinkListRoot;
    sLinkListNode* pTemp = NULL;
    while(pTempRoot != NULL) {
        pTemp = pTempRoot->pNext;
        free(pTempRoot);
        pTempRoot = pTemp;
    }
    sQueuePrivateData.pLinkListRoot = NULL;
 
    pthread_mutex_init(&sQueuePrivateData.QueueMutex, NULL);
    pthread_mutex_init(&sQueuePrivateData.LinkListMutex, NULL);
    return 0;
}
 
int QueueInit(sQueue* pQueuePrivateData)
{
    if(sQueuePrivateData.Cnt > CONFIG_QUEUE_LIMIT) {
        printf("Queue register count over limit");
        return -1;
    }
    pthread_mutex_init(&pQueuePrivateData->QueueMutex, NULL);
    pQueuePrivateData->HeadIndex = 0;
    pQueuePrivateData->TailIndex = 0;
    pthread_mutex_lock(&sQueuePrivateData.QueueMutex);
    sQueuePrivateData.QueueList[sQueuePrivateData.Cnt] = pQueuePrivateData;
    sQueuePrivateData.Cnt++;
    pthread_mutex_unlock(&sQueuePrivateData.QueueMutex);
    return 0;
}
 
 
static int QueuePushBack(sQueue* pQueuePrivateData, sQueueData* pData)
{
    int HeadIndex, TailIndex, Index;
    pthread_mutex_lock(&pQueuePrivateData->QueueMutex);
    HeadIndex = pQueuePrivateData->HeadIndex;
    TailIndex = pQueuePrivateData->TailIndex;
 
    Index = (TailIndex + 1) % CONFIG_QUEUE_SIZE;
    if(Index == HeadIndex) {
        // printf("Warn: queue full\n");
        pthread_mutex_unlock(&pQueuePrivateData->QueueMutex);
        return -1;
    } else {
        memcpy(&pQueuePrivateData->Data[TailIndex], pData, sizeof(sQueueData));
        pQueuePrivateData->TailIndex = Index;
        pthread_mutex_unlock(&pQueuePrivateData->QueueMutex);
        return 0;
    }
}
 
int QueuePutData(sQueueData* pData)
{
    int Ret = -1;
    pthread_mutex_lock(&sQueuePrivateData.QueueMutex);
 
    pthread_mutex_lock(&sQueuePrivateData.LinkListMutex);
    LinkedListAdd(pData->pData);
    pthread_mutex_unlock(&sQueuePrivateData.LinkListMutex);
    
    for(int i = 0; i < sQueuePrivateData.Cnt; i++) {
        Ret = QueuePushBack(sQueuePrivateData.QueueList[i], pData);
        if(Ret) {
            QueueCallback(pData);
        }
    }
 
    pthread_mutex_unlock(&sQueuePrivateData.QueueMutex);
    return 0;
}
 
int QueuePopData(sQueue* pQueuePrivateData, sQueueData* pData)
{
    int HeadIndex, TailIndex;
 
    pthread_mutex_lock(&pQueuePrivateData->QueueMutex);
    HeadIndex = pQueuePrivateData->HeadIndex;
    TailIndex = pQueuePrivateData->TailIndex;
    if(HeadIndex != TailIndex) {
        memcpy(pData, &pQueuePrivateData->Data[HeadIndex], sizeof(sQueueData));
        pQueuePrivateData->HeadIndex = (HeadIndex + 1) % CONFIG_QUEUE_SIZE;
        pthread_mutex_unlock(&pQueuePrivateData->QueueMutex);
        return 0;
    } else {
        pthread_mutex_unlock(&pQueuePrivateData->QueueMutex);
        return -1;
    }
}
 
int QueueCallback(sQueueData* pQueueData)
{
    pthread_mutex_lock(&sQueuePrivateData.LinkListMutex);
    LinkedListDel(pQueueData->pData);
    pthread_mutex_unlock(&sQueuePrivateData.LinkListMutex);
    return 0;
}