CFLAGS = -Wall -O2
COMPILER = g++

all: modules helpfiles

modules : calcmodul.o timemodul.o parsemodul.o gamemodul.o quastionmodul.o infomodul.o adminmodul.o logmodul.o badmodul.o mucmodul.o modul.o repositorymodul.o aliasmodul.o

helpfiles : version.o timeiq.o modulesfactory.o mucmodulesfactory.o

mucmodulesfactory.o : mucmodulesfactory.h mucmodulesfactory.cpp
	$(COMPILER) $(CFLAGS) -c mucmodulesfactory.cpp

modulesfactory.o : modulesfactory.cpp modulesfactory.h
	$(COMPILER) $(CFLAGS) -c modulesfactory.cpp

timeiq.o : timeiq.cpp timeiq.h
	$(COMPILER) $(CFLAGS) -c timeiq.cpp

version.o : version.cpp version.h
	$(COMPILER) $(CFLAGS) -c version.cpp

timemodul.o : timemodul.cpp timemodul.h
	$(COMPILER) $(CFLAGS) -c timemodul.cpp

repositorymodul.o : repositorymodul.cpp repositorymodul.h
	$(COMPILER) $(CFLAGS) -c repositorymodul.cpp

modul.o  : modul.h modul.cpp
	$(COMPILER) $(CFLAGS) -c modul.cpp

parsemodul.o : parsemodul.cpp parsemodul.h
	$(COMPILER) $(CFLAGS) -c parsemodul.cpp

calcmodul.o : calcmodul.cpp calcmodul.h
	$(COMPILER) $(CFLAGS) -c calcmodul.cpp

gamemodul.o : gamemodul.cpp gamemodul.h
	$(COMPILER) $(CFLAGS) -c gamemodul.cpp

quastionmodul.o : quastionmodul.cpp quastionmodul.h
	$(COMPILER) $(CFLAGS) -c quastionmodul.cpp

infomodul.o : infomodul.cpp infomodul.h
	$(COMPILER) $(CFLAGS) -c infomodul.cpp

adminmodul.o : adminmodul.h adminmodul.cpp
	$(COMPILER) $(CFLAGS) -c adminmodul.cpp

aliasmodul.o : aliasmodul.cpp aliasmodul.h
	$(COMPILER) $(CFLAGS) -c aliasmodul.cpp

badmodul.o : badmodul.cpp badmodul.h
	$(COMPILER) $(CFLAGS) -c badmodul.cpp

logmodul.o : logmodul.cpp logmodul.h
	$(COMPILER) $(CFLAGS) -c logmodul.cpp

mucmodul.o : mucmodul.cpp mucmodul.h
	$(COMPILER) $(CFLAGS) -c mucmodul.cpp

clean:
	rm *.o *~
