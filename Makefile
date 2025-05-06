CC = gcc

SRCS = Page.c FIFO.c LFU.c LRU.c MFU.c Random.c  main.c
OBJS = $(SRCS:.c=.o)
EXEC = main

.PHONY: all clean run

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
	rm -f *.txt

run: clean all
	./$(EXEC) FIFO > process_logs_fifo.txt
	./$(EXEC) LFU > process_logs_lfu.txt
	./$(EXEC) LRU > process_logs_lru.txt
	./$(EXEC) MFU > process_logs_mfu.txt
	./$(EXEC) Random > process_logs_random.txt