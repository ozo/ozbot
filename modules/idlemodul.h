#ifndef IDLEMODUL_H
#define IDLEMODUL_H

#include "modul.h"

#include <ctime>
#include <set>
#include <string>

class IdleModul : public Modul{
public:
    IdleModul( gloox::Client *cl, const std::set< std::string > &users );
    virtual      ~IdleModul(){}
    virtual bool Message( gloox::MUCRoom* room
			  , const std::string &normal
			  , const std::string &upper
			  , const gloox::JID &from
			  ,  bool priv );
    virtual bool IsHaveMode( const std::string& ) const;
    void         RegisterUser( std::string user );
    void         RemoveUser(   std::string user );
protected:
    std::map< std::string, std::time_t > lastActivityTimes;
};

#endif
