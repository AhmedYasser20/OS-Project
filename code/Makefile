build:
	gcc process_generator.c IO_Functions.c QueueOfProcesses.c -o process_generator.out
	gcc clk.c -o clk.out
	gcc scheduler.c PriorityQueue.c QueueOfProcesses.c ArrayOfPCB.c -o scheduler.out -lm
	gcc process.c -o process.out
	gcc test_generator.c -o test_generator.out

clean:
	rm -f *.out  processes.txt

all: clean build

run:
	./process_generator.out
