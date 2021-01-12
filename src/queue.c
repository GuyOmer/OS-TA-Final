#include <pthread.h>
#include <stdlib.h>
#include <memory.h>

#include <queue.h>
#include <common.h>
#include <status.h>
#include <stdbool.h>

typedef struct queue_node_s queue__queue_node_t;
struct queue_node_s {
    int value;
    queue__queue_node_t *next;
};

struct queue__queue_s {
    queue__queue_node_t *start;
    queue__queue_node_t *end;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} ;

bool
is_empty(queue__queue_t *queue) {
    return (queue->end == queue->start) && (queue->start == NULL);
}

void
queue__queue_destory(queue__queue_t *queue) {
    queue__queue_node_t * node = NULL;
    queue__queue_node_t * temp_node = NULL;

    if(queue == NULL) {
        return;
    }

    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->cond);

    node = queue->start;
    while(node != NULL) {
        temp_node = node;
        node = node->next;

        SAFE_FREE(temp_node);
    }
}

status_t
queue__push(queue__queue_t *queue, int value) {
    status_t retval = UNINITIALIZED;
    queue__queue_node_t *new_node = NULL;

    new_node = malloc(sizeof(*new_node));
    if(new_node == NULL) {
        retval = QUEUE__FAILED_ALLOCATING_NEW_NODE;
        goto cleanup;
    }

    if(0 != pthread_mutex_lock(&queue->mutex)) {
        retval = QUEUE__FAILED_LOCKING_MUTEX;
        goto cleanup;
    }

    // if queue is empty
    if(queue->end == NULL) {
        queue->start = new_node;
        queue->end = new_node;
    } else {
        new_node->next = queue->end;
        queue->end = new_node;
    }

    new_node->value = value;
    retval = SUCCESS;
cleanup:

    (void)pthread_mutex_unlock(&queue->mutex);
    (void)pthread_cond_signal(&queue->cond);
    return retval;
}

status_t
queue__pop(queue__queue_t *queue, int *value) {
    status_t retval = UNINITIALIZED;
    queue__queue_node_t *node = NULL;
    int inner_value = 0;

    pthread_mutex_lock(&queue->mutex);
    while(is_empty(queue)) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }

    // if queue is empty
    if(queue->start == NULL) {
        retval = QUEUE__POPING_FROM_EMPTY_QUEUE;
        goto cleanup;
    }

    if (queue->start != queue->end) {
        queue->start = queue->start->next;
    }

    node = queue->start;
    inner_value = node->value;
    SAFE_FREE(node);

    *value = inner_value;
    retval = SUCCESS;
cleanup:

    (void)pthread_mutex_unlock(&queue->mutex);
    return retval;
}

status_t
queue__initialize(queue__queue_t **queue) {
    status_t retval = UNINITIALIZED;
    queue__queue_t *inner_queue = NULL;

    inner_queue = (queue__queue_t *)malloc(sizeof(*inner_queue));
    if(NULL == inner_queue) {
        retval = QUEUE__FAILED_INITIALIZING_QUEUE;
        goto cleanup;
    }
    memset(inner_queue, 0, sizeof(*inner_queue));

    if(0 != pthread_mutex_init(&inner_queue->mutex, NULL)) {
        retval = QUEUE__FAILED_INITIALZING_MUTEX;
        goto cleanup;
    }

    if(0 != pthread_cond_init(&inner_queue->cond, NULL)) {
        retval = QUEUE__FAILED_INITIALIZING_COND;
        goto cleanup;
    }

    *queue = inner_queue;
    inner_queue = NULL;
    retval = SUCCESS;

cleanup:
    queue__queue_destory(inner_queue);
    return retval;
}
