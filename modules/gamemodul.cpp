#include "gamemodul.h"

#include "../analyzemsg.h"

#include <fstream>

#include <gloox/client.h>
#include <gloox/mucroom.h>

GameModul::GameModul( gloox::Client *cl, std::list< std::string > &rootsJids )
    : RootModul( cl, rootsJids )
    , games( 0 ){
    AddMode( "!GAME", "Запуск теста/викторины : !game start filе.test/file.quiz\n" 
	              "Текущий вопрос : !game cur\n"
	              "Статистика : !game stat\n"
	              "Остановка игры : !game stop");
}

GameModul::~GameModul(){
    if( games )
	for( std::map< gloox::MUCRoom*, Game* >::iterator i = games->begin()
		 ; i != games->end()
		 ; ++i )
	    delete i->second;
    delete games;
}

bool GameModul::Message( gloox::MUCRoom* room
			, const std::string &normal
			, const std::string &upper
			, const gloox::JID &from
			,  bool priv ){
    const int MSG_LENGTH = numberOfWords( upper );
    
    Game* cur = IsHaveGame( room );
    
    if( cur ){
	for( int i = 0; i < MSG_LENGTH; ++i )
	    if( cur->IsAnswer( getWord( upper, i ) ) ){
		DeleteModesFromString( cur->GetAnswers() );
		cur->NextQuastion( from.resource() );
		AddModesFromString( cur->GetAnswers() );
		room->send( from.resource() + " правильно ответил на вопрос, его счёт : "
			    + cur->GetUserStat( from.resource() ) );
		room->send( cur->GetQuastion() );
		return 1;
	    }
    }

    if( getWord( upper, 0 ) != "!GAME"  )
	return 0;

    if( MSG_LENGTH < 2 ){
	LowLength( room, from, priv );
	return 0;
    }

    const std::string SECOND_WORD  = getWord( upper, 1 );
    const int         MODES_LENGTH = 4;
    std::string       modes[]      = { "STAT", "CUR", "STOP", "START" };
    for( int i = 0; i < MODES_LENGTH; ++i )
	if( SECOND_WORD == modes[i] )
	    switch( i ){
	    case 0:
		Stat( room, from, priv );
		break;
	    case 1:
		Cur( room, from, priv );
		break;
	    case 2:
		if( IsRoot( from.full() ) )
		    Stop( room, from, priv );
		else
		    NotPermissed( from, room, priv );
		break;
	    case 3:
		if( MSG_LENGTH > 2 ){
		    if( IsRoot( from.full() ) ){
			try{
			    Start( room, getWord( normal, 2 ), from, priv );
			} catch( FileModul::FileNotFound ){
			    std::string msg("ошибка чтения файла");
			    if( priv )
				Send( from, msg, "game" );
			    else
				room->send( from.resource() + " : " + msg );
			}
		    }else
			NotPermissed( from, room, priv );
		}
		break;
	    default:
		break;
	    }

    return 1;
  
}

void GameModul::Start( gloox::MUCRoom *room
		       , const std::string &file
		       , const gloox::JID &from
		       , bool priv){
    Game *cur = IsHaveGame( room );
    if( cur ){
	Send( room, from, "В данной конференции уже запущена игра", priv );
	return;
    }
    if( IsGoodFile( file ) ){
	if( !games )
	    games = new std::map< gloox::MUCRoom*, Game* >;
	if( file.find( ".quiz" ) == std::string::npos )
	    cur = new TestGame( file );
	else
	    cur = new QuizGame( file );
	
	games->insert( std::make_pair( room, cur ) );
	AddModesFromString( cur->GetAnswers() );
	room->send( cur->GetQuastion() );
    }
}


void GameModul::Stop( gloox::MUCRoom *room, const gloox::JID &from, bool priv ){
    std::string msg;
    if( IsHaveGame( room ) ){
	std::map< gloox::MUCRoom*, Game* >::iterator game = games->find( room );
	DeleteModesFromString( game->second->GetAnswers() );
	delete game->second;
	games->erase( game );
	if( games->empty() ){
	    delete games;
	    games = 0;
	}
	msg = "игра становлена";
    } else
	msg = "в данной конференции не запущена игра";
    Send( room, from, msg, priv );
}


void GameModul::Stat( gloox::MUCRoom *room, const gloox::JID &from, bool priv ){
    Game *cur = IsHaveGame( room );
    std::string msg;
    if( cur )
	msg = cur->GetFullStat();
    else
	msg = "в данной конференции не запущена игра";
    Send( room, from, msg, priv );
}


void GameModul::Cur( gloox::MUCRoom *room, const gloox::JID &from, bool priv ){
    Game *cur = IsHaveGame( room );
    std::string msg;
    if( cur )
	msg = cur->GetQuastion();
    else
	msg = "в данной конференции не запущена игра";
    Send( room, from, msg, priv );
}

bool GameModul::IsGoodFile( const std::string &file) const {
    if( file.find(".quiz") == std::string::npos && file.find(".test") == std::string::npos )
	throw FileModul::FileNotFound();

    bool isHave;
    std::ifstream test( file.c_str() );
    isHave = test;
    test.close();
    if( !isHave )
	throw FileModul::FileNotFound();
    return isHave;
}

void GameModul::DeleteModesFromString( const std::string &modes ){
    for( int i = 0, length = numberOfWords( modes ); i < length; ++i )
	DeleteMode( getWord( modes, i ) );
}

void GameModul::AddModesFromString( const std::string &modes ){
    for( int i = 0, length = numberOfWords( modes ); i < length; ++i )
	AddMode( getWord( modes, i ) );
}
