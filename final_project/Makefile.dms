CC:= gcc

CFLAGS:= -Wall -O0 -g -DNODEBUG

all: shell180.x

shell180.x: shell180.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f shell180.x
