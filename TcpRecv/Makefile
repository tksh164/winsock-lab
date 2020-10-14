all: help

help:
	@type <<
nmake build    Build the executable file.
nmake clean    Clean all intermediate files.
nmake help     Show help for this Makefile. (default target)

<<

build:
	cl.exe /Zi TcpRecv.cpp

clean:
	del *.exe
	del *.obj
	del *.ilk
	del *.pdb
