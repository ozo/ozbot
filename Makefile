CFLAGS = -Wall -O2

all: other
	g++ -o bot *.o modules/*.o -lgloox -lcurl -lpthread -lgmp -lgmpxx

other : analyzemsg.o messageloger.o quastion.o quiz.o test.o main.o

analyzemsg.o : analyzemsg.cpp analyzemsg.h
	g++ $(CFLAGS) -c analyzemsg.cpp

messageloger.o : messageloger.cpp messageloger.h
	g++ $(CFLAGS) -c messageloger.cpp

quastion.o : quastion.cpp quastion.h
	g++ $(CFLAGS) -c quastion.cpp

quiz.o : quiz.cpp quiz.h
	g++ $(CFLAGS) -c quiz.cpp

test.o : test.cpp test.h
	g++ $(CFLAGS) -c test.cpp

main.o : main.cpp
	g++ $(CFLAGS) -c main.cpp
clean:
	rm *.o modules/*.o *~ bot
