#include "logmodul.h"

#include "../analyzemsg.h"
#include "../messageloger.h"

#include <gloox/client.h>
#include <gloox/mucroom.h>

LogModul::LogModul( gloox::Client *cl )
    : Modul( cl )
    , log( new MessageLoger ){
    AddMode( "!LOG", "Показать лог сообщений : !log");
}

LogModul::~LogModul(){
    delete log;
}

bool LogModul::Message( gloox::MUCRoom* room
			, const std::string &normal
			, const std::string &upper
			, const gloox::JID &from
			,  bool priv ){

    if( !IsHaveMode( getWord( upper, 0 ) ) )
	return 0;

    if( numberOfWords( upper ) == 1 ){
	Send( from, log->GetFull(room->name() ) , "log" );
	return 1;
    }

    const std::string SECOND_WORD = getWord( upper, 1 );
    if( SECOND_WORD == "FULL" )
	Send( from, log->GetFull(room->name() ) , "log" );
    else if( SECOND_WORD == "SOME" )
	Send( from, log->GetMessages( 50, room->name() ) , "log" );
    else if( SECOND_WORD == "MANY" )
	Send( from, log->GetMessages( 100, room->name() ), "log" );
    return 1;

}

void LogModul::AddToLog( const std::string &resource, const std::string &from, const std::string &msg){
    log->AddMessage( resource, from, msg );
}
