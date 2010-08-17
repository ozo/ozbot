#include "mucmodul.h"

#include "infomodul.h"
#include "adminmodul.h"
#include "badmodul.h"
#include "gamemodul.h"
#include "parsemodul.h"
#include "calcmodul.h"
#include "repositorymodul.h"
#include "logmodul.h"
#include "quastionmodul.h"
#include "idlemodul.h"
#include "aliasmodul.h"
#include "../analyzemsg.h"

#include <algorithm>

#include <gloox/client.h>
#include <gloox/mucroom.h>
#include <gloox/message.h>

MUCModul::MUCModul( gloox::Client *cl
		    , const std::list< std::string > &rootsJid
		    , const std::list< std::string > &mucJids
		    , const std::list< std::string > &defModules )
    : RootModul( cl, rootsJid )
    , init( 1 ){
    version.first = new Version( client );
    version.first->RegisterVersionHandler( this );

    gloox::MUCRoom *tmp;
    for( std::list< std::string >::const_iterator jid = mucJids.begin()
	     ; jid != mucJids.end(); ++jid ){
	tmp = new gloox::MUCRoom( client, *jid, this, 0 );
	tmp->join( gloox::Presence::Chat, "C++ круче Кольца Всевластья!");
	rooms.push_back( tmp );
    }
    
    LoadModul( "MUCROOM" );

    for( std::list< std::string >::const_iterator i = defModules.begin()
	     ; i != defModules.end(); ++i )
	LoadModul( *i );

    AddMode( "!HELP", "Получить помощь по каманде : !help <команда>" );
    AddMode( "!LSMOD", "Получить список загруженных модулей : !lsmod" );
    const int    MODES_LENGTH = 6;
    const std::string modes[] = { "НЯ", "МЯУ", "!CLEAR", "!QUIT", "!LOAD", "!UNLOAD" };
    for( int i = 0; i < MODES_LENGTH; ++i )
	AddMode( modes[ i ] );
}

MUCModul::~MUCModul(){
    for( std::map< std::string,  Modul* >::iterator modul = loadedModules.begin()
	     ;modul != loadedModules.end(); ++modul )
	if( modul->first != "MUCROOM" )
	    delete modul->second;

    for( std::list< gloox::MUCRoom* >::iterator i = rooms.begin(); i != rooms.end(); ++i ){
	(*i)->removeMUCRoomHandler( );
	delete *i;
    }
    delete version.first;
}

void MUCModul::handleMUCMessage( gloox::MUCRoom *room
			    , const gloox::Message& msg
			    , bool priv ){
    init = msg.when();

    if( msg.body().length() > 1000 )
	return;

    std::map< std::string, Modul* >::iterator logIt = loadedModules.find( "LOG" );
    bool haveLog = ( logIt != loadedModules.end() );
    LogModul *log;
    log = static_cast< LogModul* >( &(*logIt->second ) );
    if( !priv && haveLog )
    	log->AddToLog( room->name(), msg.from().resource(), msg.body() );

    if( msg.from().resource() == room->nick() || msg.body().empty() || msg.when() )
	return;

    std::string body = msg.body();
    std::map< std::string, Modul* >::iterator aliasit = loadedModules.find( "ALIAS" );
    AliasModul *aliasModul = 0;
    if( aliasit != loadedModules.end() ){
	aliasModul = static_cast< AliasModul* >( &(*aliasit->second ) );
	loadedModules.erase( aliasit );
	if( aliasModul->IsHaveMode( toUpper( getWord( body, 0 ) ) ) )
	    aliasModul->Message( room, body, toUpper( body ), msg.from(), priv );
	aliasModul->ReplaceAliases( msg.from(), body );
    }
    std::string upper = toUpper( body );

    bool aliasModulWasUnload = 0;
    for( std::map< std::string, Modul* >::iterator i = loadedModules.begin()
	     ; i != loadedModules.end(); ++i ){
	if( i->first == "MUCROOM" && aliasModul )
	    loadedModules.insert( std::make_pair( "ALIAS", aliasModul ) );

	for( int curWord = 0, length = numberOfWords( upper ); curWord < length; ++curWord )
	    if( i->second->IsHaveMode( getWord( upper, curWord ) ) ){
		i->second->Message( room, body, upper, msg.from(), priv );
		break;
	    }

	if( i->first == "MUCROOM" && aliasModul ){
	    aliasModulWasUnload = ( loadedModules.find( "ALIAS" ) == loadedModules.end() );
	    if( !aliasModulWasUnload )
		loadedModules.erase( loadedModules.find( "ALIAS" ) );
	}
    }

    if( aliasModul && !aliasModulWasUnload )
	loadedModules.insert( std::make_pair( "ALIAS", aliasModul ) );
}

bool MUCModul::Message( gloox::MUCRoom* room
		      , const std::string &normal
		      , const std::string &upper
		      , const gloox::JID &from
		      ,  bool priv ){

    const int MSG_LENGTH = numberOfWords( normal );

    for( int i = 0; i < MSG_LENGTH; ++i )
	if( getWord( upper, i ) == "НЯ" || getWord( upper, i ) == "МЯУ" ){
	    const int NAS_NUMBER    = 5;
	    const std::string nas[ NAS_NUMBER ] = { "Ня", "Мяу", "мя", "Мяяууу", "Няяяя" };
	    Send( room, from, nas[ std::rand() % NAS_NUMBER ], priv );
	    return 1;
	}

    const std::string firstWord    = getWord( upper, 0 );
    const int         MODES_NUMBER = 4;
    const std::string modes[ MODES_NUMBER ] = { "!CLEAR", "!QUIT", "!HELP", "!LSMOD" };
    bool              isHave       = 0;
    for( int i = 0; i < MODES_NUMBER; ++i )
	if( modes[i] == firstWord ){
	    switch( i ){
	    case 0:
		if( IsRoot( from.full() ) )
		    Clear( room ); 
		else
		    NotPermissed( from, room, priv );
		break;
	    case 1:
		if( IsRoot( from.full() ) ){
		    for( std::list< gloox::MUCRoom* >::iterator i = rooms.begin()
			     ; i != rooms.end(); ++i )
			(*i)->leave("Всем чмоки-поки");
		    client->disconnect();
		}else
		    NotPermissed( from, room, priv );
		break;
	    case 2:
		{
		    std::string helpMsg;
		    if( MSG_LENGTH == 1 ){
			helpMsg = "Доступные команды : ";
			std::list< std::string > tmp;
			for( std::map< std::string, Modul* >::const_iterator i = loadedModules.begin()
				 ; i != loadedModules.end(); ++i ){
			    tmp = i->second->GetModes();
			    for( std::list< std::string >::const_iterator mode = tmp.begin()
				     ; mode != tmp.end(); ++mode )
				helpMsg += *mode + ' ';
			}
			helpMsg += "\nПолучить помощь по команде : !help команда";
		    } else {
			for( std::map< std::string, Modul* >::const_iterator i = loadedModules.begin()
				 ;i != loadedModules.end(); ++i )
			    if( i->first != "BAD" 
				&& i->second->IsHaveMode( getWord( upper, 1 ) )
				&& !i->second->GetModeInfo( getWord( upper, 1 ) ).empty() ){
				bool helpMsgWasEmpty = helpMsg.empty();
				if( helpMsgWasEmpty )
				    helpMsg += '\n';
				helpMsg = i->second->GetModeInfo( getWord( upper, 1 ) );
			    }
		    }
		    if( !helpMsg.empty() )
			Send( room, from, helpMsg, priv );
		}
		break;
	    case 3:
		{
		    std::string result("В данный момент запущены модули :\n" );
		    for( std::map< std::string, Modul*>::const_iterator i = loadedModules.begin()
			     ; i != loadedModules.end(); ++i )
			result += i->first + ' ';
		    Send( room, from, result, priv );
		}
		break;
	    default:
		break;
	    }
	    isHave = 1;
	}
    if( isHave  )
	return 1;
	
    if( !IsRoot( from.full() ) ){
	NotPermissed( from, room, priv );
	return 0;
    }

    if( MSG_LENGTH < 2 ){
	LowLength( room, from, priv );
	return 0;
    }

    const std::string MODUL = getWord( upper, 1 );
    if( firstWord == "!LOAD" ){
	const std::string FILE  = ( MSG_LENGTH > 2 ) ?  getWord( normal, 2 )  : "";
	try{
	    std::string msg = "Модуль " + MODUL + " был удачно запущен";
	    if( !LoadModul( MODUL + ' ' + FILE ) )
		msg = "Модуль " + MODUL + " не был запущен";
	    Send( room, from, msg, priv );
	}catch( FileModul::FileNotFound ){
	    const std::string msg("невозможно прочитать данные из файла, "
				  "модуль " + MODUL + " не был загружен" );
	    Send( room, from, msg, priv );
	}
	return 1;
    }  

    if( firstWord == "!UNLOAD" ){
	std::string msg( "Работа модуля " + MODUL + " не была завершена" );
	if( UnLoadModul( MODUL ) )
	    msg = "Работа модуля " + MODUL + " была удачна завершена";
	Send( room, from, msg, priv );
	return 1;
    }

    return 0;
}

void MUCModul::handleMUCParticipantPresence( gloox::MUCRoom *room
					, const gloox::MUCRoomParticipant participant
					,  const gloox::Presence& presence ){
    std::map< std::string, Modul* >::iterator idleIt = loadedModules.find( "IDLE" );
    IdleModul *idle = 0;
    if( idleIt != loadedModules.end() )
	idle = static_cast< IdleModul* >( &( *idleIt->second ) );

    std::map< std::string, Modul* >::iterator qIt = loadedModules.find( "QUASTIONS" );
    QuastionModul *quastions = 0;
    if( qIt != loadedModules.end() )
	quastions = static_cast< QuastionModul* >( &( *qIt->second ) );
    
    const std::string nick = participant.nick->full();
    if( presence.presence() == gloox::Presence::Unavailable ){
	if( quastions )
	    quastions->DeleteUser( nick );
	if( inRoom.find( nick ) != inRoom.end() )
	    inRoom.erase( inRoom.find( nick ) );
	if( idle )
	    idle->RemoveUser( nick );
    } else {
	if( inRoom.find( nick ) == inRoom.end()
	    && participant.affiliation == gloox::AffiliationNone ){
	    if( !init ){
		version.first->query( nick, 0 );
		version.second.push( room );
	    }
	    room->send( participant.nick->resource() + " : ты хто ? " );
	}
	if( inRoom.find( nick ) == inRoom.end() )
	    inRoom.insert( nick );
	if( quastions )
	    quastions->AddUser( nick );
	if( idle )
	    idle->RegisterUser( nick );
    }
}

void MUCModul::HandleVersion( const Version::version &v, int context ){
    if( version.second.empty() )
	return;

    gloox::MUCRoom *room = 0;
    if( !version.second.empty() ){
	room = version.second.front();
	version.second.pop();
    }

    if( v.os.empty() )
	return;

    std::string os = toUpper( v.os );
    if( os.find( "WINDOWS" ) != std::string::npos )
	if( room ){
	    room->send( "Ахтунг, виндузятник !" );
	    room->setRole( v.jid.resource(), gloox::RoleVisitor );
	}
}

void MUCModul::HandleVersionError(){
    if( !version.second.empty() )
	version.second.pop();
}

bool MUCModul::LoadModul( const std::string &loadString ){
    const std::string MODUL = getWord( loadString, 0 );

    if( loadedModules.find( MODUL ) != loadedModules.end() )
	return 0;

    const std::string FILE = ( numberOfWords( loadString ) > 1 ) ? getWord( loadString, 1 ) : "";

    const int   MODULES_LENGTH = 12;
    std::string modules[ MODULES_LENGTH ]      = { "LOG"
						   , "QUASTIONS"
						   , "INFO"
						   , "ADMIN"
						   , "BAD"
						   , "GAME"
						   , "PARSER"
						   , "CALC"
						   , "REPO"
						   , "IDLE"
						   , "ALIAS"
						   , "MUCROOM"
                                                 };
    bool isHave = 0;
    for( int i = 0; i < MODULES_LENGTH; ++i )
	if( MODUL == modules[i] ){
	    switch( i ){
	    case 0:
		loadedModules.insert( std::make_pair( modules[i], new LogModul( client ) ) );
		break;
	    case 1:
		if( FILE.empty() )
		    return 0;
		loadedModules.insert( std::make_pair( modules[i]
						      , new QuastionModul( client, FILE ) ) );
		break;
	    case 2:
		loadedModules.insert( std::make_pair( modules[i], new InfoModul( client ) ) );
		break;
	    case 3:
		loadedModules.insert( std::make_pair( modules[i]
						      , new AdminModul( client, *roots ) ) );
		break;
	    case 4:
		if( FILE.empty() )
		    return 0;
		loadedModules.insert( std::make_pair( modules[i]
						      , new BadModul( client, FILE ) ) );
		break;
	    case 5:
		loadedModules.insert( std::make_pair( modules[i]
						      , new GameModul( client, *roots ) ) );
		break;
	    case 6:
		loadedModules.insert( std::make_pair( modules[i], new ParseModul( client ) ) );
		break;
	    case 7:
		loadedModules.insert( std::make_pair( modules[i], new CalcModul( client ) ) );
		break;
	    case 8:
		loadedModules.insert( std::make_pair( modules[i]
						      , new RepositoryModul( client, FILE ) ) );
		break;
	    case 9:
		loadedModules.insert( std::make_pair( modules[i]
						      , new IdleModul( client, inRoom ) ) );
		break;
	    case 10:
		loadedModules.insert( std::make_pair( modules[i]
						      , new AliasModul( client, FILE ) ) );
		break;
	    case 11:
		loadedModules.insert( std::make_pair( modules[i], this ) );
		break;
	    }
	    isHave = 1;
	}
    return isHave;
}

bool MUCModul::UnLoadModul( const std::string &mode ){
    if( mode == "MUCROOM" )
	return 0;

    std::map< std::string, Modul*>::iterator mod = loadedModules.find( mode );
    if( mod != loadedModules.end() ){
	delete mod->second;
	loadedModules.erase( mod );
	return 1;
    } else
	return 0;
}

void MUCModul::Clear( gloox::MUCRoom *room){
    std::string mucMsg;
    mucMsg.assign( 100, '\n' );
    room->send(mucMsg);
}
