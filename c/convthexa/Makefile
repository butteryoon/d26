
SHELL		= /bin/sh
CC		= gcc
AR		= ar
RM		= /bin/rm
MV		= mv
DEPEND		= makedepend
LINT		= lint

TOP_LIB		= ../../LIB
TOP_BIN		= ../../BIN
INSTALL_BIN	= /WNTAFAPP/BIN
LIBRARY		= -L$(TOP_LIB)

INCLUDES	= -I../../INC -I.
DEFINES		= -DPRINT
OPTIMIZE	= -g3 -Wall
DEBUG       	= -DDEBUG 

CFLAGS		= $(OPTIMIZE) $(INCLUDES) $(DEFINES) $(DEBUG) 
SRCS		= convthexa.c
TARGET		= convthexa

OBJS		= $(SRCS:.c=.o)

LIBS		= $(LIBRARY)
#LIBS		= $(LIBRARY) -lLogUtil -lSHMUTIL -lInitShm -lhasho -lmemg -ltimerN -lhashg -lgifo -lcifo -lnifo -lmems

MAKEFILE	= Makefile

#
#-----------------------------------------------------------
#

#
.SUFFIXES: .c .s .o .i .u
.s.o:
	$(CC) $(CFLAGS) -c $<
.c.s:
	$(CC) $(SFLAGS) -S $<
.c.o:
	$(CC) $(CFLAGS) -c $<
.c.u:
	$(CC) $(CFLAGS) -j $<
.c.i:
	$(CC) $(CFLAGS) -P $<


all: $(TARGET)

#$(SRCS):
#	$(GET) -s $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)
#	cp $(TARGET) $(TOP_BIN)

lint:
	lint $(INCLUDES) $(CFLAGS) -h -v -a -u -x -m $(SRCS)

depend: $(SRCS)
	$(DEPEND) -o.o -f $(MAKEFILE) -- $(INCLUDES) -- $(SRCS)

install:
	cp $(TARGET) $(TAF_BIN)

clean:
	/bin/rm -f *.o $(TARGET) $(TARGET2) core* 

# DO NOT DELETE THIS LINE -- make depend depends on it.
