
Implementing a queue library that can be used to enqueue and dequeue work items
and code to test the working of the library.

File 'prod_cons_queue.c' contains the implementation of the library.
Essentially, the file contains routines for a producer process/thread to enqueue
and the consumer process/thread to walk & process items from the queue.  
None of the work items are deleted from the queue.

File 'test_queue_implementation.c' has the code to test the working of this library.
Locking and synchronization is entirely done in the test code.
Output from the testing can be selectively printed by commenting/uncommenting print flags

'Makefile' can be used to compile the project.

The executable is 'test_queue_implementation', it can take the number of consumers as an argument in the command line. If no argument is given, it defaults to 3 consumers.

To run this code: 

Git clone the project or download the project 
#make
#./test_queue_implementation
