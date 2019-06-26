CC = gcc
CFLAGS = -Wall
LDFLAGS =
OBJFILES = prod_cons_queue.o test_queue_implementation.o
TARGET = test_queue_implementation

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~
