
#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <assert.h>
#include <memory.h>
#include <stdlib.h>

#include "util.h"

#define MAX_SIZE_QUEUE 1024

typedef struct _Queue {

	void* queue[MAX_SIZE_QUEUE];
	int   front;
	int   rear;
	int   size;

	pthread_mutex_t lock;
	pthread_cond_t  cond;

} Queue;

Queue* queue_create(void);
int queue_push(Queue *q, void *val);
void* queue_pop(Queue *q);
void queue_destroy(Queue *q);

#endif