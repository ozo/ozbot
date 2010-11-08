#ifndef CONNECTIONSTATE_H
#define CONNECTIONSTATE_H

#include <gloox/gloox.h>

class ConnectionState{
public:
    static gloox::ConnectionError GetError( ){
	return ConnectionState::error;
    }
    static void SetError( gloox::ConnectionError err ){
	ConnectionState::error = err;
    }
private:
    static gloox::ConnectionError error; // init in bot.cpp
};

#endif
