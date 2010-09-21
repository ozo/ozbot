#ifndef ADMINMODUL_H
#define ADMINMODUL_H

#include "modul.h"

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
    void SetRole(gloox::MUCRoom *room, const std::string &role, const std::string &user );

    void SetAff(gloox::MUCRoom *room, const std::string &aff, const std::string &user );

    void Kick(gloox::MUCRoom *room, const std::string &user);

    void Ban(gloox::MUCRoom *room, const std::string &user);
    
    void Say(gloox::MUCRoom *room, const std::string &msg);

    gloox::Presence::PresenceType GetPresence( const std::string &from ) const;
    //    bool IsGoodRole( gloox::MUCRoom *room, const gloox::JID &from );

};

#endif
