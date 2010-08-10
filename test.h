#ifndef TEST_H
#define TEST_H

#include "game.h"

#include <string>
#include <list>
#include <map>

class Test : public Game {
public:
    Test( const std::string &filename );
    ~Test();

    virtual bool        IsAnswer( const std::string &answer ) const;
    virtual std::string GetAnswers() const;
    virtual std::string GetQuastion() const;
    virtual void        NextQuastion( const std::string &user /* = "" */, const int points );

protected:
    struct Quastion{
	std::string quastion;
	std::list< std::string > answers;
    };
    std::list< Quastion > *quastions;
    std::list< Quastion >::iterator cur;
};

#endif
