CFLAGS = -Wall -Os

all: other
	g++ -o bot *.o modules/*.o -lgloox -lcurl -lpthread -lgmp -lgmpxx

other : analyzemsg.o quastion.o quizgame.o testgame.o bot.o main.o

analyzemsg.o : analyzemsg.cpp analyzemsg.h
	g++ $(CFLAGS) -c analyzemsg.cpp

quastion.o : quastion.cpp quastion.h
	g++ $(CFLAGS) -c quastion.cpp

quizgame.o : quizgame.cpp quizgame.h
	g++ $(CFLAGS) -c quizgame.cpp

testgame.o : testgame.cpp testgame.h
	g++ $(CFLAGS) -c testgame.cpp

bot.o : bot.cpp bot.h
	g++ $(CFLAGS) -c bot.cpp

main.o : main.cpp
	g++ $(CFLAGS) -c main.cpp

clean:
	rm *.o modules/*.o *~ bot
