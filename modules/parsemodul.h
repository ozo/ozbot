#ifndef PARSEMODUL_H
#define PARSEMODUL_H

#include <curl/curl.h>
#include <string>

#include "modul.h"

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
    enum ParseMode{
	BOR, WEATHER, IBASH, LORQ, CALENDAR
    };

    std::string GetDataFromUrl( const std::string &url ) const;

    std::string Bor( unsigned int howMany );

    std::string Weather( const std::string &city );

    CURL*       curl;
private:
    static  int         Writer( char *data, size_t size, size_t nmemb, std::string *buffer );
    mutable std::string buffer;
    char                errorBuffer[ CURL_ERROR_SIZE ];

    inline void Replace( std::string &str, const std::string &from, const std::string &to ); 
    inline void EraseWith( std::string &str, const std::string &to ); 

    std::map< std::string, std::string > *citys;
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
