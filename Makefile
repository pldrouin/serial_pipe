SRCS = $(wildcard *.c)
EXES = $(SRCS:.c=)
LDEPS = $(SRCS:.c=.d)

all: $(EXES)

$(LDEPS): %.d: %.c
	@echo "Generating dependency file $@"
	@set -e; rm -f $@
	@$(CC) -M $(CFLAGS) -MT $(<:.c=.o) $< > $@.tmp
	@sed 's,\($*\)\.o[:]*,\1 $@ : ,g' < $@.tmp > $@
	@rm -f $@.tmp

include $(LDEPS)

$(EXES): % : %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o *.d

clear: clean
	rm -f $(EXES)
