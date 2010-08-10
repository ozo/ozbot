#include "badmodul.h"

#include "../analyzemsg.h"

#include <algorithm>
#include <list>
#include <fstream>

#include <gloox/client.h>

BadModul::BadModul( gloox::Client *cl, const std::string &fileName )
    : FileModul( cl, fileName ){
    EraseComments();
    for( FileModul::StringList::const_iterator str = content.begin()
	     ; str != content.end(); ++str )
	for( int curNumber = 0; curNumber < numberOfWords( *str ); ++curNumber )
	    AddMode( getWord( *str, curNumber ) );
}

bool BadModul::Message( gloox::MUCRoom* room
	      , const std::string &normal
	      , const std::string &upper
	      , const gloox::JID &from
	      ,  bool priv ){
    
    for( std::map< std::string, std::string >::const_iterator i = modes.begin(); i != modes.end(); ++i )
	if( upper.find( i->first ) != std::string::npos ){
	    PupilsCount::iterator user = badBoys.find( from.resource() );
	    if( user != badBoys.end() ){
		user->second += 1;
		if( user->second < 10 )
		    Send( from, room, OLD, priv );
		else
		    Send( from, room, BAD, priv );
	    } else {
		badBoys.insert( std::make_pair( from.resource(), 0 ) );
		Send( from, room, NEW, priv );
	    }
	    return 1;
	}
    return 0;
}

void BadModul::Send( const gloox::JID &from
		     , gloox::MUCRoom *room
		     , Man mode
		     , const bool priv ) const {
    const std::string answers[ 3 ] = { "без матов, пожалуйста"
				       , "так тоже нужно уметь :'-("
				       , "у нас не матерятся :)"  };
    std::string answer;
    switch( mode ){
    case NEW : answer = answers[ 2 ]; break;
    case OLD : answer = answers[ 0 ]; break;
    case BAD : answer = answers[ 1 ]; break;
    default  : break;
    }
    Modul::Send( room, from, answer, priv );
}
