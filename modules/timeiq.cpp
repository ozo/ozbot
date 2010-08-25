#include "timeiq.h"

#include <gloox/jid.h>
#include <gloox/disco.h>
#include <gloox/tag.h>

#include <algorithm>
#include <unistd.h>

#include "../loger.h"

const int TIME_QUERY_NUMBER = 110;
static const std::string XMLNS_TIME = "jabber:iq:time";

TimeIq::TimeQuery::TimeQuery( const gloox::Tag* tag )
    : StanzaExtension( TIME_QUERY_NUMBER ){
    if( !tag )
	return;
    else {
	const std::string DISPLAY_TAG = "<display>";
	const std::string TZ_TAG      = "<tz>";
	std::string xml = tag->xml( );
	std::string tz;
	if( xml.find( TZ_TAG ) != std::string::npos ){
	    xml.erase( 0, xml.find( TZ_TAG ) + TZ_TAG.length( ) );
	    tz = xml.substr( 0, xml.find( "<" ) );
	}

	if( xml.find( DISPLAY_TAG ) != std::string::npos ){
	    xml.erase( 0, xml.find( DISPLAY_TAG ) + DISPLAY_TAG.length(  ) );
	    t.time = xml.substr( 0, xml.find( "<" ) );
	}
	if( t.time.find( tz ) != std::string::npos )
	    t.time.erase( t.time.find( tz ), tz.length(  ) );
    }
}

const std::string &TimeIq::TimeQuery::filterString() const {
    static const std::string filter = "/iq/query[@xmlns='" + XMLNS_TIME + "']";
    return filter;
}

gloox::StanzaExtension *TimeIq::TimeQuery::newInstance( const gloox::Tag* tag ) const{
    return ( new TimeQuery( tag ) );
}

gloox::Tag* TimeIq::TimeQuery::tag() const {
    gloox::Tag* t = new gloox::Tag( "query" );
    t->setXmlns( XMLNS_TIME );
    t->addAttribute( "time", this->t.time );
    t->setCData( "timeQuery" );
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
