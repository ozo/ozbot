#include "bot.h"

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
    , thread( 0 )
    , pingPongRequestTime( 0 )
    , badConnection( 0 ){
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

Bot::~Bot(){
    if( thread )
	pthread_detach( thread );
    j->removeConnectionListener( this );
    delete mucModul;
    delete j;
}

void Bot::Start(){
    j->connect();
}    

bool Bot::ConnectionError(){
    return badConnection;
}

void Bot::onConnect(){
    mucModul = new MUCModul( j, roots, mucs, modes );
    pthread_create(&thread, NULL, Bot::ThreadMetod, static_cast< void* >( this) );
}

void *Bot::ThreadMetod( void *tmp ){
    static_cast< Bot* >( tmp )->Ping();
    pthread_exit( 0 );
}

void Bot::Ping(){
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

void Bot::handleEvent( const gloox::Event  &event){
    pingPongRequestTime = 0;
}
