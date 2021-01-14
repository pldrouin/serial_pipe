SRCS = $(wildcard *.c)
EXES = $(SRCS:.c=)

all: $(EXES)

$(EXES): % : %.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o *.d

clear: clean
	rm -f $(EXES)
