#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>

#include "prod_cons_queue.h"

// Uncomment to print items in the queue and Walker positions
#define PRINT_QUEUE_INFO 1

// Uncomment to print producer consumer events
#define PRINT_PRODUCER_CONSUMER_EVENTS 1

#define DEFAULT_CONSUMER_COUNT 3
//Global shared queue data structure
queue_t *q;
// Lock to the queue which needs to be acquired by producer/ consumer before processing the queue
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;

// The count of consumers entering/ exiting the critical section of the code
int consumer_count = 0;

// Lock to increment the count of consumers in the critical section
pthread_mutex_t consumer_count_lock = PTHREAD_MUTEX_INITIALIZER;

//Counting semaphore that the consumer waits on to process the queue data
sem_t to_walk = 0;
// Binary semaphore which the producer waits on to create / fill the queue
sem_t to_fill = 1;

void enqueue_routine() ;

void* producer_enqueue_elements(void *number_of_consumers) ;

void *consumer_dequeue_elements(void* consumer_id) ;
