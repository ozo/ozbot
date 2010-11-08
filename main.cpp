#include <fstream>
#include <vector>
#include <algorithm>
#include <unistd.h>

#include "bot.h"
#include "connectionstate.h"
#include "loger.h"

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

inline bool IsHave( const std::list< std::string > &lst, const std::string &str ){
    return ( std::find( lst.begin( ), lst.end( ), str ) != lst.end( ) );
}

int main(){
    std::list< std::string > file;
    std::ifstream config( "config" );
    if( config ){
	std::string tmp;
	while( std::getline( config, tmp ) )
	    file.push_back( tmp );
    }
    config.close( );
    std::for_each( file.begin( ), file.end( ), deleteComment );
    while( std::find( file.begin( ), file.end( ), "" ) != file.end( ) )
	file.erase( std::find( file.begin( ), file.end( ), "" ) );

    std::list< std::string > keyWords;
    keyWords.push_back( "JID:" );
    keyWords.push_back( "PSWD:" );
    keyWords.push_back( "MUCJID:" );
    keyWords.push_back( "ROOTS:" );
    keyWords.push_back( "MODES:" );
    std::string jid, pswd;
    std::list< std::string > MUCJids, roots, modes;
    {
	std::list< std::string >::iterator tmp;
	for( std::list< std::string >::iterator i = file.begin( )
		 ; i != file.end( ); ++i )
	    if( ( tmp = std::find( keyWords.begin( ), keyWords.end( ), *i) ) != keyWords.end( ) ){
		++i;
		switch( std::distance( keyWords.begin( ), tmp ) ){
		case 0 :
		    if( i != file.end( ) && !IsHave( keyWords, *i ) ) 
			jid = *i;
		    break;
		case 1 :
		    if( i != file.end( ) && !IsHave( keyWords, *i ) )
			pswd = *i;
		    break;
		case 2 :
		    for(; i != file.end( ) && !IsHave( keyWords, *i ); ++i )
			MUCJids.push_back( *i );
		    break;
		case 3 :
		    for(; i != file.end( ) && !IsHave( keyWords, *i ); ++i )
			roots.push_back( *i );
		    break;
		case 4 :
		    for(; i != file.end( ) && !IsHave( keyWords, *i ); ++i )
			modes.push_back( *i );
		    break;
		default :
		    break;
		    }
		--i;
	    }
    }
    file.clear( );
    if( jid.empty() || pswd.empty() || modes.empty() || roots.empty() )
	DEBUG << Loger::DEBUG << "Неправильно оформлен конфигурационный файл\n"
	    "Обратитесь к документации, запуск бота невозможен\n";
    else {
	Bot *bot = 0;
	for( ;; ){
	    bot = new Bot( jid, pswd, MUCJids, roots, modes );
	    bot->Start();
	    delete bot;
	    if( ConnectionState::GetError( ) != gloox::ConnNotConnected )
		break;
	    DEBUG << Loger::DEBUG << "Restart bot, bad connection";
	    sleep( 60 );
	}
    }
}
