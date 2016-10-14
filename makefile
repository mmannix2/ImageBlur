CC=gcc
CFLAGS=-g -Wall --std=c99 -fopenmp
TARGET=image_blur

all:
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)

clean:
	rm $(TARGET)
