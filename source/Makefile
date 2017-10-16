#Makefile for placer

PLATFORM = X11

EXE = placer
HDR = log.h
FLAGS = -g -Wall -D$(PLATFORM) -std=c++11
UNAME := $(shell uname)

$(System is $(UNAME))

ifeq ($(UNAME), Darwin)
	ifeq ($(PLATFORM),X11)
	   GRAPHICS_LIBS = -lX11 -L/usr/X11/lib -I/usr/X11/include
	endif
else
	ifeq ($(PLATFORM),X11)
	   GRAPHICS_LIBS = -lX11
	endif
endif


$(EXE): graphics.o csynth.o configholder.o placer.o
	g++ $(FLAGS) graphics.o csynth.o configholder.o placer.o $(GRAPHICS_LIBS) -o $(EXE)

graphics.o: graphics.cpp $(HDR)
	g++ -c $(FLAGS) graphics.cpp

csynth.o: csynth.cpp $(HDR)
	g++ -c $(FLAGS) csynth.cpp

configholder.o: configholder.cpp $(HDR)
	g++ -c $(FLAGS) configholder.cpp

placer.o: placer.cpp $(HDR)
	g++ -c $(FLAGS) placer.cpp

clean:
	rm $(EXE) *.o