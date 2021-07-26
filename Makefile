# Diogo Paris Kraut - GRR 20166365

TARGET = caminhada
LIBS = -lm
CC = gcc
CFLAGS = -Wall -g

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

.PHONY: defult all nobound
	
default: $(TARGET)
all: default
nobound: CFLAGS += -DNOBOUND
nobound: $(TARGET)

%.o: %.c $(HEADERS)
	$(CC) $(LIBS) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(LIBS) $(CFLAGS) $(OBJECTS) -o $@

clean:
	-rm -f *.o core

purge:
	-rm -f *.o core
	-rm -f $(TARGET)