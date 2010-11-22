#include "bot.h"
#include "loger.h"
#include "modules/modul.h"

#include <unistd.h>

Bot::Bot(  const std::string jid_
	   , const std::string pswd
	   , const std::list< std::string > &mucJids
	   , const std::list< std::string > &rootJid
	   , const std::list< std::string > &mods )
    : j( 0 )
    , jid( jid_ )
    , password( pswd )
    , mucModul( 0 )
    , connectionError( 0 )
    , thread( 0 )
    , pingPongRequestTime( 0 ){
    mucs = mucJids;
    RootModul::SetRoots( rootJid );
    modes = mods;
    j = new gloox::Client( jid, password );
    j->disableRoster(); 
    j->registerConnectionListener( this );
    j->setPresence( gloox::Presence::Chat, 0, "I hate char*" );
    j->disco()->setVersion( "gloox ", gloox::GLOOX_VERSION, "Arch Linux" );
    j->disco()->setIdentity( "client", "KataJab" );
    j->setCompression( false );
}

Bot::~Bot(){
    RootModul::ClearRoots( );
    if( thread ){
	pthread_join( thread, 0 );
	pthread_detach( thread );
    }
    j->removeConnectionListener( this );
    delete mucModul;
    delete j;
}

void Bot::Start(){
    j->connect();
}    

void Bot::onConnect(){
    delete mucModul;
    mucModul = new MUCModul( j, mucs, modes );
    pthread_create(&thread, NULL, Bot::ThreadMetod, static_cast< void* >( this) );
}

void *Bot::ThreadMetod( void *tmp ){
    static_cast< Bot* >( tmp )->Ping();
    pthread_exit( 0 );
}

void Bot::Ping(){
    const int DEADLINE_TIME = 60;
    while( 1 ){
	for( int i = 0; i < 19; ++i ){
	    sleep( 1 );
	    if( j->state( ) == gloox::StateDisconnected )
		return;
	}

	if( !pingPongRequestTime ){
	    pingPongRequestTime = std::time( 0 );
	    j->xmppPing( jid, this );
	} else 
	    if( ( std::time( 0 ) - pingPongRequestTime ) > DEADLINE_TIME ){
		connectionError = 1;
		j->disconnect();
		return;
	    }
    }
}

void Bot::handleEvent( const gloox::Event  &event){
    pingPongRequestTime = 0;
}
