#
# nmake makefile for id3tool
#
CC=cl.exe
CFLAGS=/O2 /Ot /GB /QI0f /GF
VERSION=1.2a

all:	id3tool.exe

OBJS=   id3tool.obj id3.obj bsd_getopt.obj
LIBS=	setargv.obj

id3tool.exe:    $(OBJS)
        $(CC) $(CFLAGS) /ML /Fe$@ $(OBJS) $(LIBS)

.c.obj:
	$(CC) /c $(CFLAGS) /Fo$@ $<

prep:
        copy config.h.win32 config.h
        copy id3tool.mak makefile

clean:
        -del *.obj
        -del *~

distclean:
        -del makefile
        -del config.h
        -del id3tool.exe

binarydist:	id3tool.exe
	zip -9 id3tool-$(VERSION)-win32-$(PROCESSOR_ARCHITECTURE).zip COPYING README README.Win32 id3tool.exe

