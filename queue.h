#pragma once

#include<stdbool.h>

typedef struct queue_t queue_t;

queue_t *queue_create(void);

void queue_push(queue_t *q, void *val);

void *queue_front(queue_t *q);

int queue_size(queue_t *q);

bool queue_empty(queue_t *q);

void *queue_pop(queue_t *q);
