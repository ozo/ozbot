#ifndef BOT_H
#define BOT_H

#include "modules/mucmodul.h"

#include <gloox/client.h>
#include <gloox/eventhandler.h>
#include <gloox/connectionlistener.h>

#include <pthread.h>

#include <unistd.h>

#include <queue>
#include <ctime>

class Bot : public gloox::ConnectionListener, gloox::EventHandler {
public:
    Bot(  const std::string jid_
	  , const std::string pswd
	  , const std::list< std::string > &mucJids
	  , const std::list< std::string > &rootJid
	  , const std::list< std::string > &mods )
	: thread( 0 )
	, pingPongRequestTime( 0 )
	, badConnection( 0 )
	, j( 0 )
	, jid( jid_ )
	, password( pswd )
	, mucModul( 0 ){
	mucs = mucJids;
	roots = rootJid;
	modes = mods;
	j = new gloox::Client( jid, password );
	j->disableRoster(); 
	j->registerConnectionListener( this );
	j->setPresence( gloox::Presence::Chat, 0, "I hate char*" );
	j->disco()->setVersion( "gloox ", gloox::GLOOX_VERSION, "ArchLinux" );
	j->disco()->setIdentity( "client", "KataJab" );
	j->setCompression( false );
    }
    
    ~Bot(){
	if( thread )
	    pthread_detach( thread );
	j->removeConnectionListener( this );
	delete mucModul;
	delete j;
    }

    void start(){
	j->connect();
    }

    bool ConnectionError(){
	return badConnection;
    }
    
    virtual void onConnect(){
	mucModul = new MUCModul( j, roots, mucs, modes );
	pthread_create(&thread, NULL, Bot::ThreadMetod, static_cast< void* >( this) );
    }
    
    virtual void onDisconnect( gloox::ConnectionError e ){

    }

    virtual bool onTLSConnect(const gloox::CertInfo&){
	return 1;
    }
    
protected:
    pthread_t   thread;
    std::time_t pingPongRequestTime;
    bool badConnection;

    static void *ThreadMetod( void *tmp ){
	static_cast< Bot* >( tmp )->Ping();
	pthread_exit( 0 );
    }

    void Ping(){
	while( 1 ){
	    sleep( 10 );
	    if( !pingPongRequestTime ){
		pingPongRequestTime = std::time( 0 );
		j->xmppPing( jid, this );
	    } else 
		if( ( std::time( 0 ) - pingPongRequestTime ) > 60 ){
		    badConnection = 1;
		    j->disconnect();
		    return;
		}
	}
    }
	
    virtual void handleEvent( const gloox::Event  &event){
	pingPongRequestTime = 0;
    }
    
    gloox::Client *j;
    
    std::string jid;
    std::string password;
    std::list< std::string > mucs;
    std::list< std::string > roots;
    std::list< std::string > modes;
    
    MUCModul *mucModul;
};

#endif
