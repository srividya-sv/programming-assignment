#include "test_queue_implementation.h"

/*
 * Producer thread routine
 * Creates the queue & fills the queue with work items periodically
 * Takes as argument the number of consumer to create the queue
 *
 */

void* producer_enqueue_elements(void *number_of_consumers) {

    long consumer_count = (long) number_of_consumers;
    int num = 0;

    do {

        // to_fill needs to be initially set to 1
        // Producer waits on the semaphore and acquires the lock subsequently
        sem_wait(&to_fill) ;
        pthread_mutex_lock(&queue_lock);
        if(q == NULL)
            queue_create(&q, (int) consumer_count);

        sleep(1);
        // Adds in work items for now
        enqueue_routine();

        // Producer signals to all the consumers that they have data to read
        // Consumers can however read the data after the unlock which follows this signal
        for(long i = 0; i < consumer_count ; i++) {
            sem_post(&to_walk);
        }

        pthread_mutex_unlock(&queue_lock) ;
        num++ ;

      // We can also have a while(1) loop here;
      // restricting the number of times producer creates data to 5 presently
    } while(1) ;

  return (void*) 0;
}

/*
 * Consumer thread routine
 * Consumer ID is passed in as an argument
 * One thread is created for every consumer
 *
 */

void *consumer_dequeue_elements(void* consumer_id) {

    char res[200] ;
    int res_size;
    long id = (long) consumer_id;

    do {
        //Every consumer waits for producer to populate the queue
        sem_wait(&to_walk) ;

        //Lock the consumer counter before incrementing it,
        // there are going to be multiple threads accessing it
        pthread_mutex_lock(&consumer_count_lock);

        consumer_count++;

        // The first consumer that reaches this point will acquire lock to the queue
        if(consumer_count == 1)
            pthread_mutex_lock(&queue_lock) ;

        // Unlock the counter lock
        pthread_mutex_unlock(&consumer_count_lock);


        // Parse data starting from the head or where the consumer left off last time
        // Any other function that returns a boolean
        // can be put here to control stop/restart of the walker
        while(!walker_reached_queue_end(q, (int) id)) {
            memset(res, 0, 200);
            // Walk and print data read by consumer
            queue_walk(q, id, (void *)&res, &res_size) ;
            #ifdef PRINT_PRODUCER_CONSUMER_EVENTS
            printf("Consumer %ld processed : %s\n" , id, res) ;
            #endif
        }

        //Decrement consumer counter when queue walk is done for this consumer
        pthread_mutex_lock(&consumer_count_lock);
        consumer_count--;

        // The last consumer signals to producer that it can start filling the queue
        // Also unlocks the queue lock
        if(consumer_count == 0) {
            sem_post(&to_fill) ;
            pthread_mutex_unlock(&queue_lock) ;
        }

        pthread_mutex_unlock(&consumer_count_lock) ;

    } while(1) ;

  return (void *)0;
}

/*
 * This routine is used by the producer to enqueue objects in the queue
 * Enqueue in the head/tail of the queue , before an object / after an object in the queue
 * are all tested here. 
 * The queue can also be printed after the enqueue is done
 */

void enqueue_routine() {

    static int count = 0;
    char str[200] ;
    int i = 0;

    #ifdef PRINT_PRODUCER_CONSUMER_EVENTS
    printf("\nProducer enqueuing data.....");
    #endif
    //sleep(1);

    sprintf(str, "WorkItemHead#%d", count);
    queue_insert_object(&q, HEAD, NULL, 0, str, strlen(str)) ;

    for(i = count; i < count+2; i++) {
        sprintf(str, "WorkItem#%d", i);
        queue_insert_object(&q, TAIL, NULL, 0, str, strlen(str));
    }
    count = i;
    i--;
    char new_str[200] ;
    memset(new_str, '\0', 200);
    sprintf(new_str, "WorkItemBefore#%d",i);
    queue_insert_object(&q, BEFORE_OBJECT, str, strlen(str), new_str , strlen(new_str));
    memset(new_str, '\0', 200);
    sprintf(new_str, "WorkItemAfter#%d", i);
    queue_insert_object(&q, AFTER_OBJECT, str , strlen(str), new_str , strlen(new_str));

    #ifdef PRINT_PRODUCER_CONSUMER_EVENTS
    printf("Enqueue complete !\n") ;
    #endif

    #ifdef PRINT_QUEUE_INFO
      queue_print(q);
    #endif
}


int main(int argc, char *argv[]) {

    pthread_t producer_thread ;
    pthread_t consumer_thread[MAX_NUMBER_OF_WALKERS] ;
    int ret = 0;
    // Default to 3 consumers
    long number_of_consumers = DEFAULT_CONSUMER_COUNT;
    q = NULL ;

    if(argc == 2) {
        if(sscanf(argv[1], "%ld", &number_of_consumers) != 1)
          number_of_consumers = DEFAULT_CONSUMER_COUNT;
        if(number_of_consumers > MAX_NUMBER_OF_WALKERS) {
          printf("We can have only %d consumers\n", MAX_NUMBER_OF_WALKERS);
          exit(1);
        }
    }

    ret = pthread_create(&producer_thread, NULL, producer_enqueue_elements,(void *) number_of_consumers);
    if(ret) {
        printf("ERROR: return code %d\n", ret);
        exit(1);
    }
    sleep(1);
    for(long i = 0; i < number_of_consumers; i++) {
        ret = pthread_create(&consumer_thread[i], NULL, consumer_dequeue_elements, (void *) i) ;
    }


    pthread_join(producer_thread, NULL);
    for(int i = 0; i < number_of_consumers; i++) {
        pthread_join(consumer_thread[i], NULL);
    }
    exit(0);
}
