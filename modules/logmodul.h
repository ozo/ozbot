#ifndef LOGMODUL_H
#define LOGMODUL_H

#include "modul.h"

#include <list>
#include <algorithm>
#include <string>

namespace gloox{
    class MUCRoom;
    class JID;
}

class LogModul : public Modul{
public:
    LogModul( gloox::Client *cl );
    virtual ~LogModul(){}
    virtual bool Message( gloox::MUCRoom* room
			  , const std::string &normal
			  , const std::string &upper
			  , const gloox::JID &from
			  , bool priv );
    bool AddToLog( const gloox::MUCRoom *room, const gloox::JID &from, const std::string &msg );
protected:
    std::string GetMessages( const int length
			     , const std::string &resource ) const; 
    struct message{
	std::string resource;
	std::string user;
	std::string text;
    };
    std::list< message > log;
};

#endif
