#ifndef TIMEMODUL_H
#define TIMEMODUL_H

#include "modul.h"
#include "timeiq.h"
#include "../analyzemsg.h"

#include <string>
#include <queue>

namespace gloox{
    class Client;
    class MUCRoom;
}

class TimeModul : public Modul, TimeIq::TimeHandler {
public:
                 TimeModul( gloox::Client *cl );
    virtual      ~TimeModul( ){}
    virtual bool Message( gloox::MUCRoom* room
			  , const std::string &normal
			  , const std::string &upper
			  , const gloox::JID &from
			  , bool priv );
protected:
    virtual void HandleTime( const TimeIq::time &t, int context );
    virtual void HandleTimeError( );
private:
    TimeModul( const TimeModul& );
    struct Request{
	gloox::MUCRoom *room;
	bool priv;
	gloox::JID from;
    };
    std::queue< Request > requests;
    TimeIq timeIq;
};

#endif
