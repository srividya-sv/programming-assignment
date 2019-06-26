#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

#define MAX_NUMBER_OF_WALKERS 5


#define DEBUG_QUEUE 1
#ifndef DEBUG_QUEUE
#define DEBUG_QUEUE 1
#endif

typedef struct node {
  struct node *prev;
  void * work_item;
  int work_item_size;
  struct node *next;
} node_t ;

typedef struct queue {
  node_t *head;
  node_t *tail;
  int size;
  node_t *walker[MAX_NUMBER_OF_WALKERS] ;
} queue_t ;

enum InsertPosition {
  HEAD,
  TAIL,
  AFTER_OBJECT,
  BEFORE_OBJECT
} ;

node_t* queue_create_node_with_value(void *user_object, int user_object_size);

node_t* queue_create_node_with_value_and_ptrs(void *user_object, int user_object_size, node_t *prev, node_t *next);

void queue_create(queue_t **q, int number_of_consumers) ;

void queue_insert_tail(queue_t **q, void *user_object, int user_object_size);

void queue_insert_head(queue_t **q, void *user_object, int user_object_size) ;

void queue_insert_before_elem(queue_t **q, void *before_object, int before_object_size,
                                                void *user_object, int user_object_size);

void queue_insert_after_elem(queue_t **q, void *after_object, int after_object_size,
                                               void *user_object, int user_object_size);

void queue_insert_object(queue_t **q, int position, void *before_after_object,
                  int before_after_object_size, void *user_object, int user_object_size) ;

void queue_walk(queue_t *q, int consumer_id, void *result, int *result_size) ;

bool walker_reached_queue_end(queue_t *q, int id) ;

void queue_print(queue_t *q);
