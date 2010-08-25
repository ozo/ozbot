#include "timemodul.h"
#include "../analyzemsg.h"

#include <gloox/mucroom.h>
#include <gloox/client.h>

TimeModul::TimeModul( gloox::Client *cl )
    : Modul( cl )
    , timeIq( cl ){
    timeIq.RegisterTimeHandler( this );
    AddMode( "!TIME", "Узнать текущее время : !time user");
}

bool TimeModul::Message( gloox::MUCRoom* room
			 , const std::string &normal
			 , const std::string &upper
			 , const gloox::JID &from
			 , bool priv ){
    const std::string FIRST_WORD = getWord( upper, 0 );
    const int         MSG_LENGTH = numberOfWords( normal );
    if( !IsHaveMode( FIRST_WORD ) )
	return 0;

    std::string jid = from.full( );
    if( MSG_LENGTH > 1 )
	jid = room->name( ) + '@' + room->service( )
	    + '/' + getWordsFrom( normal, 1 );
    Request newRequest = { room, priv, from };
    requests.push( newRequest );
    timeIq.Query( jid, 0 );
    return 1;
}

void TimeModul::HandleTime( const TimeIq::time &t, int context ){
    if( !requests.empty( ) ){
	const Request     tmp = requests.front( );
	if( !t.time.empty(  ) ){
	    const std::string msg = "Нынче у данного пользователя : " + t.time;
	    Send( tmp.room, tmp.from, msg, tmp.priv );
	    requests.pop( );
	} else
	    HandleTimeError( );
    }
}

void TimeModul::HandleTimeError( ){
    if( !requests.empty( ) ){
	const Request tmp = requests.front( );
	Send( tmp.room, tmp.from, "Ошибка получения", tmp.priv );
	requests.pop( );
    }
}
