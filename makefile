OBJS1 	= manager.o 
OBJS2	= worker.o 
SOURCE	= manager.c worker.c 
HEADER  = def.h
OUT1  	= sniffer 
OUT2 	= worker
CC		= gcc
FLAGS   = -g -c 

all: all1 all2 

all1: $(OBJS1)
	$(CC) -g $(OBJS1) -o $(OUT1) 
	
all2: $(OBJS2)
	$(CC) -g $(OBJS2) -o $(OUT2) 

# create/compile the individual files >>separately<< 
	
manager.o: manager.c
	$(CC) $(FLAGS) manager.c -g3

worker.o: worker.c
	$(CC) $(FLAGS) worker.c -g3
	
# clean house
clean:
	rm -f $(OBJS1) $(OUT1)	
	rm -f $(OBJS2) $(OUT2)	
	rm -f *.fifo
	rm -f *.out
