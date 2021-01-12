#pragma once

#include "status.h"

typedef struct queue__queue_s queue__queue_t;

status_t
queue__initialize(queue__queue_t **queue);

void
queue__queue_destory(queue__queue_t *queue);

status_t
queue__push(queue__queue_t *queue, int value);

status_t
queue__pop(queue__queue_t *queue, int *value);