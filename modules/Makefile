CFLAGS = -Wall -Os

all: modules helpfiles

modules : calcmodul.o timemodul.o parsemodul.o gamemodul.o quastionmodul.o infomodul.o adminmodul.o logmodul.o badmodul.o mucmodul.o modul.o repositorymodul.o idlemodul.o aliasmodul.o

helpfiles : version.o timeiq.o

timeiq.o : timeiq.cpp timeiq.h
	g++ $(CFLAGS) -c timeiq.cpp

version.o : version.cpp version.h
	g++ $(CFLAGS) -c version.cpp

timemodul.o : timemodul.cpp timemodul.h
	g++ $(CFLAGS) -c timemodul.cpp

repositorymodul.o : repositorymodul.cpp repositorymodul.h
	g++ $(CFLAGS) -c repositorymodul.cpp

modul.o  : modul.h modul.cpp
	g++ $(CFLAGS) -c modul.cpp

parsemodul.o : parsemodul.cpp parsemodul.h
	g++ $(CFLAGS) -c parsemodul.cpp

calcmodul.o : calcmodul.cpp calcmodul.h
	g++ $(CFLAGS) -c calcmodul.cpp

gamemodul.o : gamemodul.cpp gamemodul.h
	g++ $(CFLAGS) -c gamemodul.cpp

quastionmodul.o : quastionmodul.cpp quastionmodul.h
	g++ $(CFLAGS) -c quastionmodul.cpp

infomodul.o : infomodul.cpp infomodul.h
	g++ $(CFLAGS) -c infomodul.cpp

adminmodul.o : adminmodul.h adminmodul.cpp
	g++ $(CFLAGS) -c adminmodul.cpp

aliasmodul.o : aliasmodul.cpp aliasmodul.h
	g++ $(CFLAGS) -c aliasmodul.cpp

badmodul.o : badmodul.cpp badmodul.h
	g++ $(CFLAGS) -c badmodul.cpp

logmodul.o : logmodul.cpp logmodul.h
	g++ $(CFLAGS) -c logmodul.cpp

idlemodul.o : idlemodul.cpp idlemodul.h
	g++ $(CFLAGS) -c idlemodul.cpp

mucmodul.o : mucmodul.cpp mucmodul.h
	g++ $(CFLAGS) -c mucmodul.cpp

clean:
	rm *.o *~
