#include "version.h"

#include <gloox/jid.h>
#include <gloox/disco.h>
#include <gloox/tag.h>

#include <algorithm>

Version::Query::Query( const gloox::Tag* tag )
    : StanzaExtension( 100 ){
    if( !tag ){
	return;
    }
    if( tag->xml().find( "<name>" ) != std::string::npos ){
	v.client = tag->xml();
	v.client.erase( 0, v.client.find( "<name>" ) + 6 );
	v.client.erase( v.client.find( "</name>" ) );
    }
    if( tag->xml().find( "<version>" ) != std::string::npos ){
	v.clientVersion = tag->xml();
	v.clientVersion.erase( 0, v.clientVersion.find( "<version>" ) + 9 );
	v.clientVersion.erase( v.clientVersion.find( "</version>" ) );
    }
    if( tag->xml().find( "<os>" ) != std::string::npos ){
	v.os = tag->xml();
	v.os.erase( 0, v.os.find( "<os>" ) + 4 );
	v.os.erase( v.os.find( "</os>" ) );
    }
}

Version::Query::~Query(){}

Version::version Version::Query::Result() const {
    return v;
}

const std::string &Version::Query::filterString() const {
    static const std::string filter = "/iq/query[@xmlns='" + gloox::XMLNS_VERSION + "']";
    return filter;
}

gloox::StanzaExtension *Version::Query::newInstance( const gloox::Tag* tag ) const{
    return ( new Query( tag ) );
}

gloox::Tag* Version::Query::tag() const {
    gloox::Tag* t = new gloox::Tag( "query" );
    t->setXmlns( gloox::XMLNS_VERSION );
    t->addAttribute( "name", v.client );
    t->addAttribute( "version", v.clientVersion );
    t->addAttribute( "os", v.os );
    t->setCData( "version" );
    return t;
}

gloox::StanzaExtension* Version::Query::clone() const {
    return ( new Query( *this ) );
}

Version::Version( gloox::Client* cl )
    : client( cl ){
    if( client ){
	client->registerStanzaExtension( new Query() );
	client->registerIqHandler( this, 100 );
	client->disco()->addFeature( gloox::XMLNS_VERSION );
    }
}

Version::~Version(){
    if( client ){
	client->disco()->removeFeature( gloox::XMLNS_VERSION );
	client->removeIqHandler( this, 100 );
	client->removeIDHandler( this );
    }
}

void Version::query( const gloox::JID& jid, const int context ){
    gloox::IQ iq( gloox::IQ::Get, jid, client->getID() );
    iq.addExtension( new Query() );
    jids.push( jid );
    client->send( iq, this, context );
}

void Version::RegisterVersionHandler( Version::VersionHandler *vh ){
    if( std::find( handlers.begin(), handlers.end(), vh ) == handlers.end() )
	handlers.push_back( vh );
}

void Version::RemoveVersionHandler( Version::VersionHandler *vh){
    std::list< VersionHandler* >::iterator handler;
    handler = std::find( handlers.begin(), handlers.end(), vh );
    if( handler != handlers.end() )
	handlers.erase( handler );
}

void Version::handleIqID( const gloox::IQ &iq, int context ){
    gloox::JID jid;

    if( !jids.empty() ){
	jid = jids.front();
	jids.pop();
    }

    const Query *q = iq.findExtension< Query >( 100 );
    if( q ){
	version v = q->Result();
	v.jid = jid;
	SendToHandlers( v, context );
    } else 
	SendErrorToHandlers();
}

void Version::SendToHandlers( const Version::version &v, const int context ){
    for( std::list< Version::VersionHandler* >::iterator i = handlers.begin()
	     ; i != handlers.end(); ++i )
	(*i)->HandleVersion( v, context );
}

void Version::SendErrorToHandlers(){
    for( std::list< Version::VersionHandler* >::iterator i = handlers.begin()
	     ; i != handlers.end(); ++i )
	(*i)->HandleVersionError();
}
