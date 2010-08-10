#include "idlemodul.h"

#include <gloox/client.h>
#include <gloox/mucroom.h>

#include <sstream>

IdleModul::IdleModul( gloox::Client *cl, const std::set< std::string > &users )
    : Modul( cl ){
    AddMode( "!IDLE", "Получить время последней активности пользователя : !idle username" );
    for( std::set< std::string >::const_iterator i = users.begin()
	     ; i != users.end(); ++i )
	RegisterUser( *i );
}

bool IdleModul::Message( gloox::MUCRoom* room
			 , const std::string &normal
			 , const std::string &upper
			 , const gloox::JID &from
			 ,  bool priv ){
    const std::string user = from.full();
    RegisterUser( user );
    
    if( getWord( upper, 0 ) != "!IDLE" )
	return 0;
    
    if( numberOfWords( upper ) < 2 ){
	LowLength( room, from, priv );
	return 0;
    }

    std::string msg = " Последнея активность пользователя в этой конференции была : ";
    const std::string nick = getWordsFrom( upper, 1 );
    if( nick == toUpper( room->nick() ) ){
	msg = "я никогда не засыпаю";
	Send( room, from, msg, priv );
	return 1;
    }
    
    std::string fullNick = room->name() + '@' + room->service() + '/' + nick;
    fullNick = toUpper( fullNick );
    if( lastActivityTimes.find( fullNick ) == lastActivityTimes.end() ){
	LowLength( room, from, priv );
	return 0;
    } else {
	std::stringstream stream;
	stream << ( std::time( 0 ) - lastActivityTimes.find( fullNick )->second );
	std::string tmp;
	stream >> tmp;
	msg += tmp + " сек. назад";
	Send( room, from, msg, priv );
	return 1;
    }
}

bool IdleModul::IsHaveMode( const std::string& ){
    return 1;
}

void IdleModul::RegisterUser( std::string user ){
    user = toUpper( user );
    if( lastActivityTimes.find( user ) == lastActivityTimes.end() )
	lastActivityTimes.insert( std::make_pair( user, std::time( 0 ) ) );
    else
	lastActivityTimes.find( user )->second = std::time( 0 );
}

void IdleModul::RemoveUser( std::string user ){
    user = toUpper( user );
    std::map< std::string, std::time_t >::iterator userIt;
    userIt = lastActivityTimes.find( user );
    if( userIt != lastActivityTimes.end() )
	lastActivityTimes.erase( userIt );
}
