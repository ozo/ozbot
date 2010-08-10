#include "messageloger.h"

void MessageLoger::AddMessage( const std::string &resource
			       , const std::string &user
			       , const std::string &message ){
    if( !resource.empty() && !user.empty() && !message.empty() ){
	Message msg;
	msg.resource = resource;
	msg.user = user;
	msg.message = message;
	log.push_front( msg );
    }
}

std::string MessageLoger::GetMessages( const int length, const std::string &resource ) const {
    std::string message;
    MessageList::const_iterator cur = log.begin();
    for( int i = 0; cur != log.end() && i < length; ++cur )
	if( cur->resource == resource ){
	    message = ( cur->user + " : " + cur->message + '\n' + message );
	    ++i;
	}

    return message;
}

std::string MessageLoger::GetFull( const std::string &resource ) const {
    return GetMessages( log.size(), resource );
}
    
