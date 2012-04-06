
MAKE		= make
SHELL		= /bin/sh
CC			= gcc
AR			= ar
RM			= /bin/rm
MV			= mv
DEPEND		= makedepend
LINT		= lint

TOP_LIB		= ../../LIB
TOP_BIN		= ../../BIN
LIBRARY 	= -L$(TOP_LIB)

INCLUDES	= -L../../LIB -I../../INC -I./ 
#DEFINES2	= -DPRINT -DNO_TCPHDR
#DEFINES		= -DPRINT -DNO_TCPHDR
OPTIMIZE	= -g3 -Wall
#OPTIMIZE	= -O
DEBUG		= -DDEBUG

CFLAGS		= $(OPTIMIZE) $(INCLUDES) $(DEFINES) $(DEBUG) 
SRCS		= pattern_main.c
TARGET		= PATTERN
GET			= sccs

OBJS		= $(SRCS:.c=.o)

#LIBS		= $(LIBRARY) -lIPC -lLogUtil -lCALLTCPSTACK -lSHMUTIL -lInitShm -lMMDBDESTIP -lMMDBDESTPORT ../EXPORT/make_nidtid.o
#LIBS    = $(LIBRARY) -lLogUtil ../EXPORT/make_nidtid.o -lInitShm -lLogUtil -lCALLTCPSTACK

#LIBS	= $(LIBRARY) -lANZ_LIB -lIPC -lSHMUTIL -lLogUtil -lInitShm -lMMDBDESTIP -lMMDBDESTPORT -lMMDBSESS -lMMDBCDR -lMMDBOBJ -lCALLTCPSTACK ../EXPORT/make_nidtid.o

#LIBS	= $(LIBRARY) -lANZ_LIB -lIPC -lSHMUTIL -lLogUtil -lInitShm -lCALLTCPSTACK ../EXPORT/make_nidtid.o
#LIBS	= $(LIBRARY) -lANZ_LIB -lSHMUTIL -lLogUtil -lInitShm -lCALLTCPSTACK ../EXPORT/make_nidtid.o
#LIBS	= $(LIBRARY) -lANZ_LIB -lSHMUTIL -lLogUtil -lInitShm -lCALLTCPSTACK -lgifo -lcifo -lnifo -lmems
#LIBS	= $(LIBRARY) -lpcap -lhasho -lmemg -lLogUtil -lSHMUTIL -lInitShm

MAKEFILE	= Makefile

#
#-----------------------------------------------------------
#

#

all: $(TARGET) 

$(SRCS):
#	$(GET) -s $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS) $(LIBRARY) $(LDFLAGS)
#	cp $(TARGET) $(TOP_BIN)/A_TCP0
#	cp $(TARGET) $(TOP_BIN)/A_TCP1

lex.http.c : http.l
	flex -Phttp -i http.l

lint:
	lint $(INCLUDES) $(CFLAGS) -h -v -a -u -x -m $(SRCS)

depend: $(SRCS)
	$(DEPEND) -o.o -f $(MAKEFILE) -- $(INCLUDES) -- $(SRCS)

notcp:
	@ echo "------------------------------"
	@ echo "BUILDING A_TCP : NO TCP HEADER"
	@ echo "------------------------------"
	$(MAKE) "DEFINES=$(DEFINES2)"

install:
#	cp $(TARGET) $(TOP_BIN)/A_TCP0
#	cp $(TARGET) $(TOP_BIN)/A_TCP1

clean:
	/bin/rm -f *.o $(TARGET) core*
#	sccs clean

new : clean $(SRCS)

# DO NOT DELETE THIS LINE -- make depend depends on it.
