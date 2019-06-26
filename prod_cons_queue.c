#include "prod_cons_queue.h"

/*
 * Function to create a queue which is a doubly linked list
 * Takes as argument the address of the queue pointer and the number of consumers
 * Creates a dummy head and a dummy tail node used for book-keeping
 * Initalizes walkers to point to the tail node
 */

void queue_create(queue_t **q, int number_of_consumers) {

    #ifdef DEBUG_QUEUE
      printf("Created queue\n");
    #endif

    *q = (queue_t *) malloc(sizeof(queue_t)) ;
    (*q)->head = queue_create_node_with_value("dummyHead", strlen("dummyHead"));
    (*q)->tail = queue_create_node_with_value_and_ptrs("dummyTail", strlen("dummyTail"), (*q)->head, NULL);
    (*q)->head->next = (*q)->tail;
    for(int i = 0; i < number_of_consumers; i++) {
        (*q)->walker[i] = (*q)->tail;
    }
    for(int i = number_of_consumers; i < MAX_NUMBER_OF_WALKERS; i++) {
        (*q)->walker[i] = NULL;
    }
    (*q)->size = 0;

}

/*
 * Wrapper to specify where to insert the object
 * Takes as argument the address to the queue pointer,
 * the insert position, the before/after object & it's size,
 * and the user object and it's size
 */
void queue_insert_object(queue_t **q, int position,
                    void *before_after_object, int before_after_object_size,
                    void *user_object, int user_object_size) {

    switch(position) {
      case HEAD:
          queue_insert_head(q, user_object, user_object_size);
          break;
      case TAIL:
          queue_insert_tail(q, user_object, user_object_size);
          break;
      case AFTER_OBJECT:
          queue_insert_after_elem(q, before_after_object, before_after_object_size, user_object, user_object_size);
          break;
      case BEFORE_OBJECT:
          queue_insert_before_elem(q, before_after_object, before_after_object_size, user_object, user_object_size);
          break;
      default:
          break;
    }

}

/*
 * Function to insert a work item at tail
 * Takes as arguments the address of the queue pointer, the work item to be inserted & it's size
 * Creates a queue if the queue is not already created
 * Update walkers if they are at tail
 */

void queue_insert_tail(queue_t **q, void *user_object, int user_object_size) {

    if(*q == NULL) {
        queue_create(q, MAX_NUMBER_OF_WALKERS);
    }

    node_t *curr = queue_create_node_with_value_and_ptrs(user_object, user_object_size, (*q)->tail->prev, (*q)->tail);

    // Update walkers
    for(int i = 0; i < MAX_NUMBER_OF_WALKERS; i++) {
        if ((*q)->walker[i] == NULL)
            break;
        if((*q)->walker[i] == (*q)->tail) {
            (*q)->walker[i] = curr;
        }
    }

    (*q)->tail->prev->next = curr;
    (*q)->tail->prev = curr;
    (*q)->size++;

}
/*
 * Function to insert a work item at head
 * Takes as arguments the address of the queue pointer, the work item to be inserted & it's size
 * Creates a queue if the queue is not already created
 * Update walkers if they are at head
 */

void queue_insert_head(queue_t **q, void *user_object, int user_object_size) {


    if(*q == NULL) {
        queue_create(q, MAX_NUMBER_OF_WALKERS);
    }

    node_t *curr = queue_create_node_with_value_and_ptrs(user_object, user_object_size, (*q)->head, (*q)->head->next);

    // Update walkers
    for(int i = 0; i < MAX_NUMBER_OF_WALKERS; i++) {
        if ((*q)->walker[i] == NULL)
            break;
        if((*q)->walker[i] == (*q)->head->next) {
            (*q)->walker[i] = curr;
        }
    }
    // Update other pointers
    (*q)->head->next->prev = curr;
    (*q)->head->next = curr;
    (*q)->size++;

}
/*
 * Function to insert a work item before an element
 * Takes as arguments the address of the queue pointer,
 * the work item before which to insert, it's size,
 * the work item to be inserted & it's size
 * Returns from the function if the queue is not already created
 * Update walkers if they are before the "before_object"
 */

void queue_insert_before_elem(queue_t **q, void *before_object, int before_object_size, void *user_object, int user_object_size) {

    if(*q == NULL) {
        return;
    }

    // If the first element is the before object

    if(memcmp(before_object, (*q)->head->next->work_item, before_object_size) == 0) {
        queue_insert_head(q, user_object, user_object_size);
        return;
    }

    node_t *curr = (*q)->head->next;
    node_t *new_elem = NULL;

    while(curr != NULL && curr->next != NULL) {
        if (memcmp(curr->next->work_item, before_object, before_object_size) == 0) {
            new_elem = queue_create_node_with_value_and_ptrs(user_object, user_object_size, curr, curr->next);
            // Update walkers as well
            for(int i = 0; i < MAX_NUMBER_OF_WALKERS; i++) {

                if((*q)->walker[i] == NULL)
                  break ;

                if((*q)->walker[i] == curr->next) {
                  (*q)->walker[i] = new_elem ;
                }

            }

            curr->next->prev = new_elem;
            curr->next = new_elem;
            (*q)->size++;
            break ;
        }
        curr = curr->next ;
    }
}
/*
 * Function to insert a work item after an element
 * Takes as arguments the address of the queue pointer,
 * the work item after which to insert, it's size,
 * the work item to be inserted & it's size
 * Returns from the function if the queue is not already created
 * Update walkers if they are before the "after_object"
 */

void queue_insert_after_elem(queue_t **q, void *after_object, int after_object_size, void *user_object, int user_object_size) {

    if(*q == NULL) {
        return;
    }

    // If the insert needs to happen after the last element
    if(memcmp(after_object, (*q)->tail->prev->work_item, after_object_size) == 0) {
        queue_insert_tail(q, user_object, user_object_size);
        return;
    }

    //
    node_t *curr = (*q)->head->next;
    node_t *new_elem;
    while(curr != NULL && curr->next != NULL) {
        if (memcmp(curr->work_item, after_object, after_object_size) == 0) {
            new_elem = queue_create_node_with_value_and_ptrs(user_object, user_object_size, curr, curr->next);

            // Update walkers as well
            for(int i = 0; i < MAX_NUMBER_OF_WALKERS; i++) {
                if((*q)->walker[i] == NULL) {
                    break ;
                }
                if((*q)->walker[i] == curr->next) {
                    (*q)->walker[i] = new_elem ;
                }
            }

            curr->next->prev = new_elem;
            curr->next = new_elem;
            (*q)->size++;
            break ;
        }
        curr = curr->next ;
    }
}

 /*
  * Function to walk the queue
  * Takes as argument a pointer to the queue, the walker ID and
  * fills the value of result in result & it's size in result_size
  *
  */

void queue_walk(queue_t *q, int walker_id, void *result, int *result_size) {

    if(q == NULL)
      return;

    if(walker_id >= MAX_NUMBER_OF_WALKERS)
        return ;

    node_t *curr_walker = q->walker[walker_id];

    if(curr_walker == NULL)
        return;

    if(curr_walker == q->tail)
        return ;

    if(curr_walker == q->head && curr_walker->next == q->tail)
        return ;

    memcpy(result, curr_walker->work_item, curr_walker->work_item_size) ;
    *result_size = curr_walker->work_item_size;
    q->walker[walker_id] = q->walker[walker_id]->next;
    return ;
}

/*
 * Function to print the Size of the Queue, doubly linked list, the walker's position
 * Takes as argument the queue pointer
 *
 */

void queue_print(queue_t *q) {
    node_t *curr = q->head->next;
    printf("\nQUEUE: ");
    char str[200];
    while(curr != q->tail) {
        snprintf(str, curr->work_item_size+1, "%s", curr->work_item);
        printf("%s ", str);
        if(curr->next != q->tail)
            printf("-> ");
        curr = curr->next;
    }

    printf("\nWALKERS : ");

    for(int i = 0; i < MAX_NUMBER_OF_WALKERS; i++) {
        if (q->walker[i] == NULL)
            break ;
        else
            printf("Walker%d @ %s ; ", i, q->walker[i]->work_item);
    }
    printf("\nQUEUE SIZE: %d\n", q->size) ;
    printf("\n");
}

/*
 * Function to see if the walker has reached the end of the queue
 * Returns true when the walker has reached end of list
 * Takes as argument the queue pointer and walker ID
 *
 */
bool walker_reached_queue_end(queue_t *q, int id) {
    return (q->walker[id]->next == NULL) ? 1 : 0;
}

/*
 * Creates a Doubly Linked List node to be inserted into the queue
 * Fills in data for user object and it's size as well
 * Assigns NULL to previous and next pointers
 * Returns node
 */

node_t* queue_create_node_with_value(void *user_object, int user_object_size) {
    node_t *new_node = (node_t *) malloc(sizeof(node_t));
    new_node->prev = NULL;
    new_node->next = NULL;
    new_node->work_item = malloc(sizeof(user_object_size));
    memset(new_node->work_item, 0 , user_object_size);
    memcpy(new_node->work_item, user_object, user_object_size);
    new_node->work_item_size = user_object_size;
    return new_node;
}
/*
 * Creates a Doubly Linked List node to be inserted into the queue
 * Fills in data for user object and it's size as well
 * Assigns values to previous and next pointers
 * Returns node
 */
node_t* queue_create_node_with_value_and_ptrs(void *user_object, int user_object_size, node_t *prev, node_t *next) {
    node_t *new_node = (node_t *) malloc(sizeof(node_t));
    new_node->prev = prev;
    new_node->next = next;
    new_node->work_item = malloc(sizeof(user_object_size));
    memset(new_node->work_item, 0 , user_object_size);
    memcpy(new_node->work_item, user_object, user_object_size);
    new_node->work_item_size = user_object_size;
    return new_node;
}
