#ifndef INFOMODUL_H
#define INFOMODUL_H

#include "modul.h"
#include "version.h"

#include <gloox/vcardmanager.h>
#include <gloox/vcardhandler.h>
#include <gloox/eventhandler.h>

#include <queue>
#include <sys/time.h>

namespace gloox{
    class Client;
    class MUCRoom;
}

class InfoModul : public Modul
		, gloox::VCardHandler
		, gloox::EventHandler
		, Version::VersionHandler {
public:
                        InfoModul( gloox::Client *cl );
    virtual             ~InfoModul(){}
    virtual        bool Message( gloox::MUCRoom* room
				 , const std::string &normal
				 , const std::string &upper
				 , const gloox::JID &from
				 , bool priv );
protected:
    virtual        void handleVCard( const gloox::JID& jid, const gloox::VCard* vcard );
    inline virtual void handleVCardResult( gloox::VCardHandler::VCardContext context
				    , const gloox::JID& jid
				    , gloox::StanzaError se  );
    virtual        void handleEvent( const gloox::Event  &event);
    inline         void GetVcard( const std::string &user );
    inline         void GetError();
                   void HandleVersion(const Version::version &v, int context );
    inline         void HandleVersionError();
    gloox::VCardManager vcardMng;

private:
    struct Request{
	gloox::MUCRoom *room;
	bool           priv;
	gloox::JID     from;
    };
    std::queue< Request > requests;
    std::queue< timeval > pingTimes;

    Version             version;
    void Send( std::string msg );

};

void InfoModul::handleVCardResult( gloox::VCardHandler::VCardContext context
				   , const gloox::JID& jid
				   , gloox::StanzaError se  ){
    GetError();
}

void InfoModul::GetVcard( const std::string &user ){
    vcardMng.fetchVCard( user, this );
}

void InfoModul::GetError(){
    Send( "Не удалось получить информацию" );
}

void InfoModul::HandleVersionError(){
    GetError();
}

#endif
