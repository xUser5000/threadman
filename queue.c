#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>

#include "queue.h"

typedef struct node {
    void *val;
    struct node *next;
} node_t;

struct queue_t {
    node_t *head;
    node_t *tail;
    int size;
};

queue_t *queue_create(void) {
    queue_t *q = (queue_t *) malloc(sizeof(queue_t));
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

// tail <- ... <- n2 <- n1 < head
void queue_push(queue_t *q, void *val) {
    node_t *n = (node_t *) malloc(sizeof(node_t));
    n->val = val;
    n->next = NULL;
    if (q->size > 0) {
        q->tail->next = n;
        q->tail = n;
    } else {
        q->head = n;
        q->tail = q->head;
    }
    q->size++;
}

void *queue_front(queue_t *q) {
    return q->head->val;
}

int queue_size(queue_t *q) {
    return q->size;
}

bool queue_empty(queue_t *q) {
    return q->size == 0;
}

void *queue_pop(queue_t *q) {
    if (q->size == 0) {
        return NULL;
    }

    node_t *old_head = q->head;
    void *res = old_head->val;

    q->head = q->head->next;
    free(old_head);
    q->size--;
    if (q->size == 0) q->tail = NULL;

    return res;
}
