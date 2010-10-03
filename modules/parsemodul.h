#ifndef PARSEMODUL_H
#define PARSEMODUL_H

#include <curl/curl.h>
#include <string>

#include "modul.h"
#include "commandfactory.h"

namespace gloox{
    class MUCRoom;
}

class ParseModul : public Modul{
public:
    ParseModul( gloox::Client *cl );
    virtual ~ParseModul();
    virtual bool Message( gloox::MUCRoom* room
			  , const std::string &normal
			  , const std::string &upper
			  , const gloox::JID &from
			  ,  bool priv );
protected:
    std::string GetDataFromUrl( gloox::MUCRoom*, const std::string &url );
    std::string Bor( gloox::MUCRoom*, const std::string &number );
    std::string Weather( gloox::MUCRoom*, const std::string &cmd );
    std::string Wiki( gloox::MUCRoom*,const std::string &quaryString );
    CURL*       curl;
private:
    static  int         Writer( char *data, size_t size, size_t nmemb, std::string *buffer );
    mutable std::string buffer;
    char                errorBuffer[ CURL_ERROR_SIZE ];
    inline void Replace( std::string &str, const std::string &from, const std::string &to ); 
    inline void EraseWith( std::string &str, const std::string &to ); 

    std::map< std::string, std::string > *citys;
    CommandFactory< ParseModul, std::string, const std::string& > commands;
    std::map< std::string, std::string > values;
};
    
void ParseModul::Replace( std::string &str, const std::string &from, const std::string &to ){
    std::string::size_type pos = str.find( from );
    while( pos != std::string::npos ){
	str.replace( pos, from.length(), to );
	pos = str.find( from, pos + 1 );
    }
}

void ParseModul::EraseWith( std::string &str, const std::string &to ){
    std::string::size_type pos = str.find( to );
    if( pos != std::string::npos )
	str.erase( 0, pos + to.length() );
}

#endif
