#include "repositorymodul.h"

#include "../analyzemsg.h"

#include <fstream>
#include <algorithm>

#include <gloox/client.h>
#include <gloox/mucroom.h>

RepositoryModul::RepositoryModul( gloox::Client *cl, const RepositoryModul::string &fileName )
    : FileModul( cl, fileName ){
    EraseComments();
    string distributions;
    for( FileModul::StringList::iterator i = content.begin(); i != content.end(); ++i )
	if( IsDistributionName( *i ) )
	    distributions += i->substr( 2 ) + ' ';
    AddMode( "!REPO"
	     , "Получить список репозиториев для данного дистрибутива : !repo дистрибутив\n"
	       " Список доступных дистрибутивов : " + distributions );
}

bool RepositoryModul::Message( gloox::MUCRoom* room
		      , const RepositoryModul::string &normal
		      , const RepositoryModul::string &upper
		      , const gloox::JID &from
		      , bool priv ){
    if( numberOfWords( normal ) < 2 ){
	LowLength( room, from, priv );
	return 0;
    }

    string result;

    if( getWord( upper, 0 ) == "!REPO" ){
	const string distribution = getWordsFrom( upper, 1 );
	result = GetRepositoryList( distribution );
    } else 
	return 0;
    
    if( result.empty() )
	result = "По данному дистрибутиву нет информации";
    Send( from, result, "!repo" );

    return 1;
}

RepositoryModul::string RepositoryModul::GetRepositoryList( const RepositoryModul::string &distribution ) const {
    string answer;
    for( FileModul::StringList::const_iterator i = content.begin(); i != content.end(); ++i )
	if( IsDistributionName( *i ) && i->find( distribution ) != string::npos ){
	    ++i;
	    while( i != content.end() && !IsDistributionName( *i ) ){
		answer += *i + '\n';
		++i;
	    }
	    return answer;
	}
    return answer;
}
