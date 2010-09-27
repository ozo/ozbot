#include "logmodul.h"

#include "../analyzemsg.h"

#include <gloox/client.h>
#include <gloox/mucroom.h>

LogModul::LogModul( gloox::Client *cl )
    : Modul( cl ){
    AddMode( "!LOG", "Показать лог сообщений : !log");
}

bool LogModul::Message( gloox::MUCRoom* room
			, const std::string &normal
			, const std::string &upper
			, const gloox::JID &from
			,  bool priv ){

    if( !IsHaveMode( getWord( upper, 0 ) ) )
	return 0;

    if( numberOfWords( upper ) == 1 ){
	Send( from, GetMessages( log.size( ), room->name() ), "log" );
	return 1;
    }

    const std::string SECOND_WORD = getWord( upper, 1 );
    if( SECOND_WORD == "FULL" )
	Send( from, GetMessages( log.size( ), room->name() ) , "log" );
    else if( SECOND_WORD == "SOME" )
	Send( from, GetMessages( 50, room->name() ) , "log" );
    else if( SECOND_WORD == "MANY" )
	Send( from, GetMessages( 100, room->name() ), "log" );
    return 1;

}

bool LogModul::AddToLog( const gloox::MUCRoom *room
			 , const gloox::JID &from
			 , const std::string &text ){
    if( !text.empty() ){
	message tmp;
	tmp.resource = room->name( );
	tmp.user = from.resource( );
	tmp.text = text;
	log.push_front( tmp );
	return 1;
    }
    return 0;
}

std::string LogModul::GetMessages( const int length
				   , const std::string &resource ) const {
    std::string result;
    std::list< message >::const_iterator cur = log.begin();
    for( int i = 0; cur != log.end() && i < length; ++cur )
	if( cur->resource == resource ){
	    result = ( cur->user + " : " + cur->text + '\n' + result );
	    ++i;
	}
    return result;
}
