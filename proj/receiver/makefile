CC=gcc
RM=rm -f

CFLAGS=-g -Wall -I. -D_GNU_SOURCE -D_REENTRANT
LDFLAGS=-g
LDLIBS=

#Depend file stuff
DEPFILE=.depend
DEPTEXT='\#MAKE .DEPEND FILE'

#Defining sources and objects
SRCS=$(wildcard *.c)
OBJS=$(subst .c,.o,$(SRCS))

all: main

main: $(OBJS)
	$(CC) $(LDFLAGS) -o receiver.out $(OBJS) $(LDLIBS)

depend: $(DEPFILE)

$(DEPFILE): $(SRCS)
	$(RM) $(DEPFILE)
	echo $(DEPTEXT) > $(DEPFILE)
	$(CC) $(CFLAGS) -MM $^>>$(DEPFILE);

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ $(DEPFILE)

include $(DEPFILE)