
#include "queue.h"

Queue* queue_create(void) {

	Queue *q = NULL;

	if ((q = (Queue*) malloc(sizeof(Queue))) != NULL) {
		q->front = 0;
		q->rear  = 0;
		q->size  = 0;

		pthread_mutex_init(&q->lock, NULL);
		pthread_cond_init(&q->cond, NULL);

		memset(q->queue, 0, sizeof(void*) * MAX_SIZE_QUEUE);
	}

	return q;
}

int queue_push(Queue *q, const void *str)  {

	int res = FALSE, newpos;

	assert(q != NULL && str != NULL);

	pthread_mutex_lock(&q->lock);

	if (q->size < MAX_SIZE_QUEUE) {
		q->queue[q->front] = str;
		q->front = (q->front + 1) % MAX_SIZE_QUEUE;
		q->size++;
		
		if (q->size == 1) {
			pthread_cond_broadcast(&q->cond);
		}

		res = TRUE;
	}

	pthread_mutex_unlock(&q->lock);

	return res;
}

void* queue_pop(Queue *q) {

	void *res = NULL;

	pthread_mutex_lock(&q->lock);

	if (q->size == 0) {
		pthread_cond_wait(&q->cond, &q->lock);
	}
	
	if (q->size > 0) {
		res = q->queue[q->rear];
		q->rear = (q->rear + 1) % MAX_SIZE_QUEUE;
		q->size--;
	}

	pthread_mutex_unlock(&q->lock);

}

void queue_destroy(Queue *q) {

	assert(q != NULL);

	pthread_cond_destroy(&q->cond);
	pthread_mutex_destroy(&q->lock);	
}