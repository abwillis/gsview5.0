# pstotxt2.mak
# Makefile for pstotxt2.dll, for use with OS/2 GSview

# makefile created by
# Russell Lang, 1996-07-29
# updated 1996-10-11

DRIVE=
COMP=gcc -Zomf -Zhigh-mem -Zargs-wild
FLAGS=-Zdll -Zhigh-mem -D__DLL__

COMPDIR=$(COMPBASE)\bin
INCDIR=$(C_INCLUDE_PATH)
LIBDIR=$(LIBRARYPATH)

all:	pstotxt2.dll pstotxt2.exe

.c.o:
	$(COMP) $(FLAGS) -c $*.c

ocr.h: ocr.ps mkrch.exe
	mkrch $*.ps $*.h 1

rot270.h: rot270.ps mkrch.exe
	mkrch $*.ps $*.h 2

rot90.h: rot90.ps mkrch.exe
	mkrch $*.ps $*.h 3

mkrch.exe: mkrch.c
	$(COMP) -o $*.exe $*.c

pstotxt2.o: ptotdll.c ptotdll.h
	$(COMP) $(FLAGS) -c -o pstotxt2.o ptotdll.c

pstotxt2.rc:  ocr.h rot270.h rot90.h
	copy ocr.h+rot270.h+rot90.h pstotxt2.rc

pstotxt2.res: pstotxt2.rc
	rc -i $(INCDIR) -r $*.rc

pstotxt2.dll: pstotxt2.o pstotxt2.def pstotxt2.res
	$(COMP) $(FLAGS) -o $*.dll $*.o $*.def
	rc $*.res $*.dll

pstotxt2.exe: pstotxtd.c
	$(COMP) -o pstotxtd.exe pstotxtd.c
	-del pstotxt2.exe
	rename pstotxtd.exe pstotxt2.exe

prezip: all
	copy pstotxt2.dll ..\pstotxt2.dll
	copy pstotxt2.exe ..\pstotxt2.exe
	copy pstotext.txt ..\pstotext.txt

clean:
	-del pstotxtd.exe
	-del pstotxt2.exe
	-del pstotxt2.dll
	-del pstotxt2.res
	-del pstotxt2.rc
	-del *.o
	-del ocr.h
	-del rot270.h
	-del rot90.h
	-del mkrch.exe
