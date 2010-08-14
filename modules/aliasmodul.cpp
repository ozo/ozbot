#include "aliasmodul.h"
#include "../analyzemsg.h"

#include <gloox/mucroom.h>

AliasModul::AliasModul( gloox::Client *cl, const std::string &fileName )
    : FileModul( cl, fileName ){
    AddMode( "!ALIAS", "Создать синоним выражения : !alias синоним выражение, "
	     "чувствительно к регистру" );
    AddMode( "!ALIASLS", "Получить список синонимов для себя : !aliasls" );
    AddMode( "!ALIASGL", "Получить список глобальных синонимов : !aliasgl" ); 
    AddMode( "!ALIASRM", "Удалить синоним : !aliasrm синоним" );
    EraseComments();
    for( StringList::iterator i = content.begin(); i != content.end(); ++i )
	if( numberOfWords( *i ) > 1 )
	    globalAlias.insert( std::make_pair( getWord( *i, 0 ), getWordsFrom( *i, 1 ) ) );
}

bool AliasModul::Message( gloox::MUCRoom* room
				  , const std::string &normal
				  , const std::string &upper
				  , const gloox::JID &from
				  , bool priv ){

    const std::string FIRST_WORD = getWord( upper, 0 );
    if( !IsHaveMode( FIRST_WORD ) )
	return 0;

    const int         MODES_NUMBER = 4;
    const std::string modes[ MODES_NUMBER ] = { "!ALIAS", "!ALIASLS", "!ALIASGL", "!ALIASRM" };
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
		    std::string msg = "Для вас определены синонимы :";
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
	    case 2:
		{ 
		    std::string msg = "Определены следующие глобальные синонимы :";
		    for( Aliases::const_iterator i = globalAlias.begin()
			     ; i != globalAlias.end(); ++i )
			msg += '\n' + i->first + " : " + i->second;
		    Send( room, from, msg, priv );
		}
		break;
	    case 3 :
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
    for( Aliases::const_iterator i = globalAlias.begin()
	     ; i != globalAlias.end(); ++i )
	if( str.find( i->first ) != std::string::npos )
	    str.replace( str.find( i->first ), i->first.length(), i->second );

    if( jidsAlias.find( jid.full() ) == jidsAlias.end() )
	return;

    Aliases aliasMap = jidsAlias.find( jid.full() )->second;
    for( Aliases::const_iterator i = aliasMap.begin()
	     ; i != aliasMap.end(); ++i )
	if( str.find( i->first ) != std::string::npos )
	    str.replace( str.find( i->first ), i->first.length(), i->second );
}
