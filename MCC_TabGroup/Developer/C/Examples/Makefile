CC	= vc
CFLAGS	=						\
	+aos68k						\
	-c99						\
	-warn=-1					\
	-dontwarn=51,81,163,166,167,168,170,306,307	\
	-O4						\
	-cpu=68000					\
	-I../Include					\
	-lamiga						\
	-lauto

DEMO	= ../../../Demos/TabGroup-Demo
SRCS	= TabGroup-Demo.c

all: $(DEMO)

clean:
	rm $(DEMO)

.PHONY: $(DEMO)
$(DEMO):
	$(CC) -o $@ $(SRCS) $(CFLAGS)
