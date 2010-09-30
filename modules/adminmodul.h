#ifndef ADMINMODUL_H
#define ADMINMODUL_H

#include "modul.h"
#include "commandfactory.h"

#include <list>
#include <string>

#include <gloox/presence.h>

class AdminModul : public RootModul{
public:
    AdminModul( gloox::Client *cl );
    virtual ~AdminModul( ){}
    virtual bool Message( gloox::MUCRoom* room
			  , const std::string &normal
			  , const std::string &upper
			  , const gloox::JID &from
			  , bool priv );

protected:
    bool SetRole(gloox::MUCRoom *room, const std::string &cmd );
    bool SetAff(gloox::MUCRoom *room, const std::string &cmd );
    bool SetSubject( gloox::MUCRoom *room, const std::string &cmd );
    bool SetStatus( gloox::MUCRoom *room, const std::string &cmd );
    bool Kick(gloox::MUCRoom *room, const std::string &user);
    bool Ban(gloox::MUCRoom *room, const std::string &user);
    bool Say(gloox::MUCRoom *room, const std::string &msg);
    gloox::Presence::PresenceType GetPresence( const std::string &from ) const;
    //    bool IsGoodRole( gloox::MUCRoom *room, const gloox::JID &from );
private:
    CommandFactory< AdminModul > commands;
};

#endif
