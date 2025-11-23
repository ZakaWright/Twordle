CC = gcc
CFLAGS = -Wall -Wshadow -g
LDFLAGS = -lncurses
TARGET = twordle

SRCS = game.c file_read.c state.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

game.o: game.c file_read.h state.h

file_read.o: file_read.c file_read.h state.h

state.o: state.c state.h

clean:
	rm -f $(OJBS) $(TARGET)
