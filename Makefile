CFLAGS = -Wall -O2
COMPILER = g++

all: other
	$(COMPILER) -o bot *.o modules/*.o -lgloox -lcurl -lpthread -lgmp -lgmpxx

other : analyzemsg.o quizgame.o testgame.o bot.o main.o

analyzemsg.o : analyzemsg.cpp analyzemsg.h
	$(COMPILER) $(CFLAGS) -c analyzemsg.cpp

quizgame.o : quizgame.cpp quizgame.h
	$(COMPILER) $(CFLAGS) -c quizgame.cpp

testgame.o : testgame.cpp testgame.h
	$(COMPILER) $(CFLAGS) -c testgame.cpp

bot.o : bot.cpp bot.h
	$(COMPILER) $(CFLAGS) -c bot.cpp

main.o : main.cpp
	$(COMPILER) $(CFLAGS) -c main.cpp

clean:
	rm *.o modules/*.o *~ bot
