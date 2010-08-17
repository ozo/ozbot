#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "bot.h"
#include "loger.h"

#include <unistd.h>

int isHaveKeyWord( const std::string String, const std::string *keyWords, const int keyWordsLength ){
    for( int i = 0; i < keyWordsLength; ++i )
	if( keyWords[i] == String )
	    return i + 1;
    return 0;
}

inline bool isCommented( const  std::string &String ){
    return ( String[0] == '#' );
}

void deleteComment( std::string &String ){
    std::string::size_type commentPos = String.find('#');
    if( commentPos != std::string::npos )
    	String.erase( commentPos );
    std::string::size_type emptyPos = String.find_last_not_of(' ');
    if( emptyPos != std::string::npos )
	String.erase( emptyPos + 1);
    else
	String.clear();
}

int main(){
    std::string jid, pswd, tmpString;

    std::list< std::string > mucJids;
    std::list< std::string > roots;
    std::list< std::string > modes;

    std::vector< std::string > file;
    
    std::ifstream config("config");
    while( std::getline( config, tmpString ) )
	file.push_back( tmpString );
    config.close();
    // delete comments
    std::for_each( file.begin(), file.end(), deleteComment );
    // delete empty strings
    file.erase( std::remove( file.begin(), file.end(), "" ), file.end() );

    const int KEY_WORDS_MAX = 5;
    std::string keyWords[]  = { "JID:", "PSWD:", "MUCJID:", "ROOTS:", "MODES:" };

    std::vector< std::string >::const_iterator current = file.begin();

    for(; current != file.end() && current + 1 != file.end(); ++current )
	if( isHaveKeyWord( *current, keyWords, KEY_WORDS_MAX ) ){
	    switch( isHaveKeyWord( *current, keyWords, KEY_WORDS_MAX ) ){
	    case 1:
		++current;
		jid = *current;
		break;
	    case 2:
		++current;
		pswd = *current;
		break;
	    case 3:
		while( !isHaveKeyWord( *(current + 1), keyWords, KEY_WORDS_MAX ) 
		       && current + 1 != file.end() ){
		    ++current;
		    mucJids.push_back( *current );
		}
		break;
	    case 4:
		while( !isHaveKeyWord( *(current + 1), keyWords, KEY_WORDS_MAX ) 
		       && current + 1 != file.end() ){
		    ++current;
		    roots.push_back( *current );
		}
		break;
	    case 5:
		while( !isHaveKeyWord( *(current + 1), keyWords, KEY_WORDS_MAX ) 
		       && current + 1 != file.end() ){
		    ++current;
		    modes.push_back( *current );
		}
		break;
	    default: break;
	    }
	}
    file.clear();

    if( jid.empty() || pswd.empty() || modes.empty() || roots.empty() )
	std::cout << " Неправильно оформлен конфигурационный файл\n"
		  << " Обратитесь к документации, запуск бота невозможен\n";
    else {
	Bot *bot = 0;
	for( ;; ){
	    bot = new Bot( jid, pswd, mucJids, roots, modes );
	    bot->Start();
	    const bool isError = bot->ConnectionError();
	    delete bot;
	    if( !isError )
		break;
	    DEBUG << Loger::DEBUG << "Restart bot, bad connection";
	    sleep( 60 );
	}
    }
}
