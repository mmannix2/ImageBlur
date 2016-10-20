CC=gcc
CFLAGS=-g -Wall --std=c99 -fopenmp
TARGET=image_blur

.PHONY: all clean

all: $(TARGET)

threads num:
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET) -D THREADS=${THREADS}

$(TARGET):
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)

clean:
	rm $(TARGET)
