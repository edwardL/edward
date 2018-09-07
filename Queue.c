#include <malloc/malloc.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

/*
    循环队列，用数组实现
    */
typedef struct Queue
{
    int * pBase;
    int front;  //指向头节点
    int rear;   // 指向最后一个元素的下一个节点
}QUEUE;

//函数声明
void initQueue(QUEUE *pQueue);
bool isEmpty(QUEUE *pQueue);
bool isFull(QUEUE *pQueue);
bool enQueue(QUEUE *pQueue, int value);
bool outQueue(QUEUE *pQueue, int *pValue);
void traverseQueue(QUEUE * pQueue);

int main(void)
{
    int value;
    QUEUE queue;
    
    initQueue(&queue);
    // 调用出队函数
    enQueue(&queue, 1);
    enQueue(&queue, 2);
    enQueue(&queue, 3);
    enQueue(&queue, 4);
    enQueue(&queue, 5);
    enQueue(&queue, 6);
    enQueue(&queue, 7);
    enQueue(&queue, 8);

    //调用便利队列的函数
    traverseQueue(&queue);

    if(outQueue(&queue,&value))
    {
        printf("出队一次，元素为:%d\n",value);
    }
    traverseQueue(&queue);

    if(outQueue(&queue,&value))
    {
        printf("出队一次，元素为:%d\n",value);
    }
    traverseQueue(&queue);

    getchar();
    return 0;
}

//初始化函数的实现
void initQueue(QUEUE * pQueue)
{
    // 分配内存
    pQueue->pBase = (int*)malloc(sizeof(int) * 6);
    pQueue->front = 0;
    pQueue->rear = 0;
    return;
}

//入队函数
bool enQueue(QUEUE *pQueue, int value)
{
    if(isFull(pQueue))
    {
        printf("队列已满，不能再插入元素了！\n");
        return false;
    }
    else
    {
        //向队列中添加新元素
        pQueue->pBase[pQueue->rear] = value;
        pQueue->rear = (pQueue->rear + 1) % 6;
        printf("aaskd %d\n",pQueue->rear);
        return true;
    }
}

// 出队函数
bool outQueue(QUEUE *pQueue, int *pValue)
{
    // 如果队列为空，则返回false
    if(isEmpty(pQueue))
    {
        printf("队列为空，出队失败\n");
        return false;
    }
    else
    {
        *pValue = pQueue->pBase[pQueue->front];
        pQueue->front = (pQueue->front + 1) % 6;
        return true;
    }
}

//便利队列的函数实现
void traverseQueue(QUEUE * pQueue)
{
    int i = pQueue->front;
    printf("遍历队列：\n");
    while(i != pQueue->rear)
    {
        printf("%d ",pQueue->pBase[i]);
        i = (i + 1) % 6;
    }
    printf("\n");
    return;
}

// 判断队列是否满
bool isFull(QUEUE * pQueue)
{
    if((pQueue->rear + 1) % 6 == pQueue->front)
        return true;
    else
        return false;
}

bool isEmpty(QUEUE * pQueue)
{
    if(pQueue->front == pQueue->rear)
        return true;
    else
        return false;
}