#ifndef QUIZMODUL_H
#define QUIZMODUL_H

#include "modul.h"
#include "../game.h"
#include "../quiz.h"
#include "../test.h"

#include <list>
#include <map>
#include <algorithm>

class QuizModul : public RootModul{
public:
    QuizModul( gloox::Client *cl, std::list< std::string > &roots);
    virtual ~QuizModul();
    virtual bool Message( gloox::MUCRoom* room
			  , const std::string &normal
			  , const std::string &upper
			  , const gloox::JID &from
			  ,  bool priv );
    virtual std::string GetModeInfo( const std::string &mode ) const {
	if( mode != "!GAME" )
	    return "";
	return modes.find( mode )->second;
    }
protected:
    QuizModul( const QuizModul &from );
    inline Game* IsHaveGame( gloox::MUCRoom *room) const;
    bool         IsGoodFile( const std::string &file ) const;
    inline void  DeleteMode( const std::string &mode );
    inline void  DeleteModesFromString( const std::string &modes );
    inline void  AddModesFromString( const std::string &modes );

    void Start( gloox::MUCRoom *room
		, const std::string &file
		, const gloox::JID &from
		, bool priv );
    void Stop(  gloox::MUCRoom *room, const gloox::JID &from, bool priv );
    void Stat(  gloox::MUCRoom *room, const gloox::JID &from, bool priv );
    void Cur(   gloox::MUCRoom *room, const gloox::JID &from, bool priv );

    std::map< gloox::MUCRoom*, Game* > *games;
};

Game* QuizModul::IsHaveGame( gloox::MUCRoom *room ) const {
    if( !games )
	return 0;
    
    return ( ( games->find( room ) != games->end() ) ? games->find( room )->second : 0 );
}

void QuizModul::DeleteMode( const std::string &mode ){
    if( modes.find( mode ) != modes.end() )
	modes.erase( modes.find( mode ) );
} 

#endif
