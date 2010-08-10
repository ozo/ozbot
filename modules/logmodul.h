#ifndef LOGMODUL_H
#define LOGMODUL_H

#include "modul.h"

#include <list>
#include <algorithm>

class MessageLoger;

class LogModul : public Modul{
public:
    LogModul( gloox::Client *cl );
    virtual ~LogModul();
    virtual bool Message( gloox::MUCRoom* room
			  , const std::string &normal
			  , const std::string &upper
			  , const gloox::JID &from
			  ,  bool priv );
    void AddToLog( const std::string &resource, const std::string &from, const std::string &msg);
protected:
    MessageLoger             *log;
};

#endif
