#ifndef TESTGAME_H
#define TESTGAME_H

#include "game.h"

#include <string>
#include <list>
#include <map>

class TestGame : public Game {
public:
    TestGame( const std::string &filename );
    virtual ~TestGame( ){}
    virtual bool        IsAnswer( const std::string &answer ) const;
    virtual std::string GetAnswers() const;
    virtual std::string GetQuastion() const;
    virtual void        NextQuastion( const std::string &user /* = "" */, const int points );

protected:
    struct Quastion{
	std::string quastion;
	std::list< std::string > answers;
    };
    std::list< Quastion >           quastions;
    std::list< Quastion >::iterator cur;
};

#endif
