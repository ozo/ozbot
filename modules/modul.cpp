#include "modul.h"

#include <gloox/message.h>
#include <gloox/messagesession.h>
#include <gloox/mucroom.h>
#include <gloox/client.h>
#include <gloox/jid.h>

#include <cstdlib>
#include <fstream>
#include <algorithm>

std::string Modul::GetModeInfo( const std::string &mode ){
    if( modes.find( mode ) == modes.end() ) 
	return "";
    return modes.find( mode )->second;
}

std::list< std::string > Modul::GetModes() const {
    std::list< std::string > result;
    for( std::multimap< std::string, std::string >::const_iterator i = modes.begin()
	     ; i != modes.end(); ++i )
	result.push_back( i->first );
    return result;
}

void Modul::Send( const gloox::JID &to, const std::string &msg, const std::string theme ) const {
    gloox::MessageSession *session = new gloox::MessageSession( client, to);
    session->send( msg, theme );
    client->disposeMessageSession( session );
}

void Modul::Send( gloox::MUCRoom *room
	       , const gloox::JID &to
	       , const std::string &msg
		  , bool priv ) const {
    const std::string::size_type MAX_MUC_MSG_LENGTH = 150;
    if( priv )
	Send( to, msg );
    else
	if( msg.length(  ) > MAX_MUC_MSG_LENGTH ){
	    Send( room, to, "Сообщение отправлено в личку", priv );
	    Send( to, msg );
	}else
	    room->send( to.resource() + ": " + msg );
}

void Modul::LowLength( gloox::MUCRoom *muc
		       , const gloox::JID &from
		       , const bool priv ) const {
    std::string msg = "неправильно заданы параметры";
    if( priv )
	Send( from, msg, "" );
    else
	muc->send( from.resource() + ": " + msg );
} 
RootModul::RootModul( gloox::Client *cl )
    : Modul( cl ){
}

void RootModul::NotPermissed( const gloox::JID &user, gloox::MUCRoom *room, const bool priv ){
    const int MESSAGES_NUMBER = 4;
    const std::string messages[] = {   "Вы не в праве повелевать мной :P"
				     , "Сударь, что Вы себе позволяете?"
				     , "укуси мой зад :P"
				     , "лизни тунца :P" };
    const std::string msg = messages[ std::rand() % MESSAGES_NUMBER ];
    if( priv )
	Send( user, msg, "" );
    else
	room->send( user.resource() + ": " + msg );
}

RootModul::RootList RootModul::roots;

FileModul::FileModul( gloox::Client *cl, const FileModul::string &fileName )
    : Modul( cl ){
    std::ifstream file( fileName.c_str() );
    string tmp;
    if( file )
	while( std::getline( file, tmp ) )
	    content.push_back( tmp );
    else
	throw FileNotFound();
    file.close();
}



void FileModul::EraseComments(){
    for_each( content.begin(), content.end(), EraseComment );
    content.erase( std::remove( content.begin(), content.end(), "" ), content.end() );
}

void FileModul::EraseComment( string &str ){
    string::size_type pos = str.find( '#' );
    if( pos != string::npos )
	str.erase( pos );
    pos = str.find_last_not_of( ' ' );
    if( pos != string::npos )
	str.erase( pos + 1 );
    else
	str.erase();
}
