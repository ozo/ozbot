#ifndef MUCMODUL_H
#define MUCMODUL_H

#include "modul.h"

#include <gloox/mucroomhandler.h>

#include "version.h"

#include <list>
#include <set>
#include <queue>

namespace gloox{
    class Message;
}

class MUCModul : public RootModul
	       , gloox::MUCRoomHandler
	       , Version::VersionHandler {
public:
    MUCModul( gloox::Client *cl
	      , const std::list< std::string > &mucJids
	      , const std::list< std::string > &defModules );
    
    virtual ~MUCModul();

    virtual bool Message( gloox::MUCRoom* room
			  , const std::string &normal
			  , const std::string &upper
			  , const gloox::JID &from
			  ,  bool priv );
protected:
    virtual void handleMUCMessage( gloox::MUCRoom* room, const gloox::Message& msg, bool priv );
    virtual void handleMUCParticipantPresence( gloox::MUCRoom * room
					       , const gloox::MUCRoomParticipant participant
					       ,  const gloox::Presence& presence );
    virtual void handleMUCSubject( gloox::MUCRoom *room
				   , const std::string& nick
				   , const std::string& subject ){}
    virtual void handleMUCError( gloox::MUCRoom * room, gloox::StanzaError error ){}
    virtual void handleMUCInfo( gloox::MUCRoom * room
				, int features
				, const std::string& name
				, const gloox::DataForm* infoForm ){}
    virtual void handleMUCItems( gloox::MUCRoom * room, const gloox::Disco::ItemList& items ){}
    virtual void handleMUCInviteDecline( gloox::MUCRoom *room
					 , const gloox::JID& invitee
					 , const std::string& reason ){}
    virtual bool handleMUCRoomCreation( gloox::MUCRoom *room ){
	return 1;
    }

    virtual void HandleVersion( const Version::version &v, int context = 0);
    virtual void HandleVersionError();

    bool LoadModul( const std::string &loadString );
    bool UnLoadModul( const std::string &mode );

    bool                                                 init;
    std::pair< Version*, std::queue< gloox::MUCRoom* > > version;
    std::list< gloox::MUCRoom* >                         rooms;
    std::map< std::string, Modul* >                      loadedModules;
    std::set< std::string >                              inRoom;
};

#endif
