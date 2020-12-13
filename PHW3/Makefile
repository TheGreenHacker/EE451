CC = gcc
CFLAGS = -lrt -fopenmp

default: p1_serial p2_serial p1a p1b p2 p3 

p1_serial:
	$(CC) $(CFLAGS) p1_serial.c -o p1_serial

p2_serial:
	$(CC) $(CFLAGS) p2_serial.c -o p2_serial

p1a:
	$(CC) $(CFLAGS) p1a.c -o p1a

p1b:
	$(CC) $(CFLAGS) p1b.c -o p1b

p2:
	$(CC) $(CFLAGS) p2.c -o p2

p3:
	$(CC) p3.c -o p3 -lpthread

clean:
	rm -f p1_serial p2_serial p1a p1b p2 p3 