CC=gcc

CFLAGS=-Wall -Wextra -g3
LFLAGS=

OBJS=crc32.o filemanager.o filetree.o main.o mm.o strings.o vector.o 
DEPS=crc32.h filemanager.h filetree.h mm.h strings.h vector.h
LIBS=

BIN=inotify_dev

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) $(BIN)
