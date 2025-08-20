CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

TARGET = ../build/graph
SOURCES = graph.c rpn.c validate.c stack.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = rpn.h validate.h stack.h

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p ../build
	$(CC) $(OBJECTS) -o $(TARGET)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild