#include "aliasmodul.h"
#include "../analyzemsg.h"

#include <gloox/mucroom.h>

AliasModul::AliasModul( gloox::Client *cl )
    : Modul( cl ){
    AddMode( "!ALIAS", "Создать синоним выражения : !alias синоним выражение, "
	     "чувствительно к регистру" );
}

bool AliasModul::Message( gloox::MUCRoom* room
				  , const std::string &normal
				  , const std::string &upper
				  , const gloox::JID &from
				  , bool priv ){
    if( getWord( upper, 0 ) != "!ALIAS" )
	return 0;
    if( numberOfWords( upper ) < 3 ){
	LowLength( room, from, priv );
	return 0;
    }
    const std::string alias = getWord( normal, 1 );
    const std::string str   = getWordsFrom( normal, 2 );
    if( jidsAlias.find( from.full() ) == jidsAlias.end() ){
	Aliases tmp;
	tmp.insert( std::make_pair( alias, str ) );
	jidsAlias.insert( std::make_pair( from.full(), tmp ) );
    } else
	jidsAlias.find( from.full() )->second.insert( std::make_pair( alias, str ) );
    return 1;
}

void AliasModul::ReplaceAliases( const gloox::JID &jid, std::string &str ) const {
    if( jidsAlias.find( jid.full() ) == jidsAlias.end() )
	return;
    const Aliases &aliasMap = jidsAlias.find( jid.full() )->second;
    for( Aliases::const_iterator i = aliasMap.begin()
	     ; i != aliasMap.end(); ++i )
	if( str.find( i->first ) != std::string::npos )
	    str.replace( str.find( i->first ), i->first.length(), i->second );
}
