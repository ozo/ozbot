#include "aliasmodul.h"
#include "../analyzemsg.h"

#include <gloox/mucroom.h>

AliasModul::AliasModul( gloox::Client *cl )
    : Modul( cl ){
    AddMode( "!ALIAS", "Создать синоним выражения : !alias синоним выражение, "
	     "чувствительно к регистру" );
    AddMode( "!ALIASLS", "Волучить список синонимов для себя : !aliasls" );
    AddMode( "!ALIASRM", "Удалить синоним : !aliasrm синоним" );
}

bool AliasModul::Message( gloox::MUCRoom* room
				  , const std::string &normal
				  , const std::string &upper
				  , const gloox::JID &from
				  , bool priv ){

    const std::string FIRST_WORD = getWord( upper, 0 );
    if( !IsHaveMode( FIRST_WORD ) )
	return 0;

    const int         MODES_NUMBER = 3;
    const std::string modes[ MODES_NUMBER ] = { "!ALIAS", "!ALIASLS", "!ALIASRM" };
    for( int i = 0; i < MODES_NUMBER; ++i )
	if( modes[ i ] == FIRST_WORD )
	    switch( i ){
	    case 0 :
		{
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
		    } else {
			Aliases &aliasMap = jidsAlias.find( from.full() )->second;
			if( aliasMap.find( alias ) != aliasMap.end() )
			    aliasMap.find( alias )->second = str;
			else
			    jidsAlias.find( from.full() )->second.insert( 
							      std::make_pair( alias, str ) );
		    }
		}
		break;
	    case 1 :
		{
		    std::string msg = "Для вас заданы синонимы :";
		    if( jidsAlias.find( from.full() ) != jidsAlias.end() ){
			const Aliases &aliasMap = jidsAlias.find( from.full() )->second;
			for( Aliases::const_iterator i = aliasMap.begin()
				 ; i != aliasMap.end(); ++i )
			    msg += '\n' + i->first + " : " + i->second;
		    } else 
			msg = "Вы не определили ни одного синонима";
		    Send( room, from, msg, priv );
		}
		break;
	    case 2 :
		{
		    if( numberOfWords( upper ) < 2 ){
			LowLength( room, from, priv );
			return 0;
		    }
		    const std::string alias = getWord( normal, 1 );
		    std::string msg = "Синоним \"" + alias + "\" был успешно удалён";
		    if( jidsAlias.find( from.full() ) == jidsAlias.end() ){
			msg = "Вы не определили ни одного синонима";
		    } else {
			Aliases &aliasMap = jidsAlias.find( from.full() )->second;
			if( aliasMap.find( alias ) != aliasMap.end() )
			    aliasMap.erase( aliasMap.find( alias ) );
			else
			    msg = "Вы не определили такого синонима";
		    }
		    Send( room, from, msg, priv );
		}
		break;
	    default :
		break;
	    }
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
