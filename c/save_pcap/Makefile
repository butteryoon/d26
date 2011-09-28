
SHELL	= /bin/sh

CC 		= gcc
AR 		= ar 
RM 		= /bin/rm
DEPEND	= makedepend
LINT 	= lint

TOP_BIN = 

KERNEL_VER=$(shell uname -r)
KERNEL_DIR=/lib/modules/$(KERNEL_VER)/source/include
INCLUDES = -I../../INC -I$(KERNEL_DIR) -I./
DEFINES = 
#OPTIMIZE= -O3 -Wall
#DEBUG   = -g3 -DDEBUG -Wall -DMICRO_LOG
ifeq ($(AQUA_DEBUG),)
DEBUG   = -g3 -Wall
else
DEBUG   = $(AQUA_DEBUG)
endif

CFLAGS 	= $(OPTIMIZE) $(INCLUDES) $(DEFINES) $(DEBUG)
SFLAGS 	= $(INCLUDES) $(DEFINES) $(DEBUG)
DEPOPTS = $(INCLUDES) $(DEFINES) 

HDRS 	= 

SRCS 	= pktsave.c

TARGET 	= libPKTTEMP.a 

OBJSV 	= $(SRCS:.c=s.o)
OBJCL 	= $(SRCS:.c=c.o)

#
#-----------------------------------------------------------
#

#
.SUFFIXES: .c .s s.o c.o p.o q.o .i .u .H
.s.o:
	$(CC) $(CFLAGS) -c $<
.c.s:
	$(CC) $(SFLAGS) -S $<
.cs.o:
	$(CC) -o $@ $(CFLAGS) -c $<
.cc.o:
	$(CC) -o $@ $(CFLAGS) -c $<
.c.u:
	$(CC) $(CFLAGS) -j $<
.c.i:
	$(CC) $(CFLAGS) -P $<

all: $(TARGET) $(HADS)


$(TARGET): $(SRCS) $(HDRS) $(OBJSV) 
	$(RM) -f $@
	$(AR) clq $@ $(OBJSV)
	$(AR) ts $@ 
	cp $@ ../../LIB

clean:
	$(RM) -f *.s *.u *.o *.i core $(TARGET)

depend: $(SRCS)
	$(DEPEND) -o.o -f Makefile -- $(DEPOPTS) -- $(SRCS)

lint:
	$(LINT) $(INCLUDES) -h -v -a -u -x -m $(SRCS)

# DO NOT DELETE THIS LINE -- make depend depends on it.

