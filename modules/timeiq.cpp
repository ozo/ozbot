#include "timeiq.h"

#include <gloox/jid.h>
#include <gloox/disco.h>
#include <gloox/tag.h>

#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <unistd.h>

const int TIME_QUERY_NUMBER = 110;
static const std::string XMLNS_TIME = "urn:xmpp:time";

TimeIq::TimeQuery::TimeQuery( const gloox::Tag* tag )
    : StanzaExtension( TIME_QUERY_NUMBER ){
    if( tag ){
	const std::string UTC_TAG = "<utc>";
	const std::string TZ_TAG  = "<tzo>";
	std::string xml = tag->xml( );

	bool isMoreNull = 0;
	std::string tzoHousesString;
	std::string tzoMinutsString;
	if( xml.find( TZ_TAG ) != std::string::npos ){
	    xml.erase( 0, xml.find( TZ_TAG ) + TZ_TAG.length( ) );
	    tzoHousesString = xml.substr( 0, xml.find( ':' ) );
	    isMoreNull = ( tzoHousesString[ 0 ] == '+' );
	    if( isMoreNull || tzoHousesString[ 0 ] == '-' )
		tzoHousesString.erase( 0, 1 );
	    if( tzoHousesString[ 0 ] == '0' )
		tzoHousesString.erase( 0, 1 );
	    xml.erase( 0, xml.find( ':' ) + 1 );
	    tzoMinutsString = xml.substr( 0, xml.find( '<' ) );
	    if( tzoMinutsString[ 0 ] == '0' )
		tzoMinutsString.erase( 0, 1 );
	}

	int tzoHouses = 0;
	int tzoMinuts = 0;
	if( !tzoHousesString.empty( ) && !tzoMinutsString.empty( ) ){
	    tzoHouses = boost::lexical_cast< int >( tzoHousesString );
	    if( tzoMinutsString != "0" )
		tzoMinuts = boost::lexical_cast< int >( tzoMinutsString );
	}
	if( xml.find( UTC_TAG ) != std::string::npos ){
	    xml.erase( 0, xml.find( UTC_TAG ) + UTC_TAG.length(  ) );
	    xml.erase( 0, xml.find( 'T' ) + 1 );
	    int houses = boost::lexical_cast< int >( xml.substr( 0, xml.find( ':'  ) ) );
	    xml.erase( 0, xml.find( ':' ) + 1 );
	    int minuts = boost::lexical_cast< int >( xml.substr( 0, xml.find( ':' ) ) );
	    xml.erase( 0, xml.find( ':' ) + 1 );
	    const std::string seconds = xml.substr( 0, xml.find( 'Z' ) );

	    if( tzoHouses < 0 ){
		minuts -= tzoMinuts;
		if( minuts < 0 ){
		    --houses;
		    minuts += 60;
		}
	    } else {
		minuts += tzoMinuts;
		if( minuts > 60 ){
		    ++houses;
		    minuts -=60;
		}
	    }
	    
	    houses += tzoHouses;
	    if( houses > 24 )
		houses -= 24;
	    else if( houses < 0 )
		houses += 24;
	    std::string h = boost::lexical_cast< std::string >( houses );
	    std::string m = boost::lexical_cast< std::string >( minuts );
	    if( h.length( ) == 1 )
		h = '0' + h;
	    if( m.length( ) == 1 )
		m = '0' + m;
	    t.time = h + ':' + m + ':' + seconds;
	}
    }
}

const std::string &TimeIq::TimeQuery::filterString() const {
    static const std::string filter = "/iq/time[@xmlns='" + XMLNS_TIME + "']";
    return filter;
}

gloox::StanzaExtension *TimeIq::TimeQuery::newInstance( const gloox::Tag* tag ) const{
    return ( new TimeQuery( tag ) );
}

gloox::Tag* TimeIq::TimeQuery::tag() const {
    gloox::Tag* t = new gloox::Tag( "time" );
    t->setXmlns( XMLNS_TIME );
    t->addAttribute( "time", this->t.time );
    t->setCData( "time" );
    return t;
}

gloox::StanzaExtension* TimeIq::TimeQuery::clone() const {
    return ( new TimeQuery( *this ) );
}

TimeIq::TimeIq( gloox::Client* cl )
    : client( cl )
    , curJID( 0 )
    , quarysCount( 0 ){
    if( client ){
	client->registerStanzaExtension( new TimeQuery() );
	client->registerIqHandler( this, TIME_QUERY_NUMBER );
	client->disco()->addFeature( XMLNS_TIME );
    }
}

TimeIq::~TimeIq(){
    if( client ){
	client->disco()->removeFeature( XMLNS_TIME );
	client->removeIqHandler( this, TIME_QUERY_NUMBER );
	client->removeIDHandler( this );
    }
    delete curJID;
}

void TimeIq::Query( const gloox::JID& jid, const int context ){
    const int sleepLimit = 2;
    gloox::IQ iq( gloox::IQ::Get, jid, client->getID() );
    iq.addExtension( new TimeQuery() );
    if( !curJID ){
	curJID = new std::pair< gloox::JID, std::time_t >( jid, std::time( 0 ) );
    } else {
	sleep( sleepLimit );
	if( curJID )
	    if( quarysCount > 0 ){
		quarysCount--;
		SendErrorToHandlers();
	    }
	delete curJID;
	curJID = new std::pair< gloox::JID, std::time_t >( jid, std::time( 0 ) );
    }
    quarysCount++;
    client->send( iq, this, context );
}

void TimeIq::RegisterTimeHandler( TimeIq::TimeHandler *th ){
    if( std::find( handlers.begin(), handlers.end(), th ) == handlers.end() )
	handlers.push_back( th );
}

void TimeIq::RemoveTimeHandler( TimeIq::TimeHandler *th){
    std::list< TimeHandler* >::iterator handler;
    handler = std::find( handlers.begin(), handlers.end(), th );
    if( handler != handlers.end() )
	handlers.erase( handler );
}

bool TimeIq::handleIq( const gloox::IQ& iq ){ 
    return 0;
}

void TimeIq::handleIqID( const gloox::IQ &iq, int context ){
    gloox::JID jid;

    if( !quarysCount )
	return;

    quarysCount--;
    if( !curJID ){
	SendErrorToHandlers();
	return;
    }

    const TimeQuery *q = iq.findExtension< TimeQuery >( TIME_QUERY_NUMBER );
    if( q ){
	time t = q->Result();
	t.jid = curJID->first;
	delete curJID;
	curJID = 0;
	SendToHandlers( t, context );
    } else {
	SendErrorToHandlers();
    }
}

void TimeIq::SendToHandlers( const TimeIq::time &t, const int context ){
    for( std::list< TimeIq::TimeHandler* >::iterator i = handlers.begin()
	     ; i != handlers.end(); ++i )
	(*i)->HandleTime( t, context );
}

void TimeIq::SendErrorToHandlers(){
    for( std::list< TimeIq::TimeHandler* >::iterator i = handlers.begin()
	     ; i != handlers.end(); ++i )
	(*i)->HandleTimeError();
}
