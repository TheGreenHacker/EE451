CC = nvcc

default: p1 p2 vector_add mm

p1:
	$(CC) p1.cu -o p1

p2:
	$(CC) p2.cu -o p2

vector_add:
	$(CC) vector_add.cu -o vector_add

mm:
	gcc mm.c -o mm

clean:
	rm -f p1 p2 vector_add mm