CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS = -lm
TARGET = csvstat.exe

SRCS = src/main.c src/cli.c src/csv.c src/types.c src/stats.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /f /q src\*.o $(TARGET) 2>nul || exit 0