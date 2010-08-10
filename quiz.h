#ifndef QUIZ_H
#define QUIZ_H

#include "game.h"

#include <string>
#include <map>

class Quiz : public Game{
public:
    Quiz( const std::string &filename );
    virtual ~Quiz(){
    }
    virtual bool        IsAnswer( const std::string &answer ) const;
    virtual std::string GetAnswers() const;
    virtual std::string GetQuastion() const;
    virtual void        NextQuastion( const std::string &user /* = "" */, const int points );
protected:
    std::map< const std::string, std::string> quastions;
    std::map< const std::string, std::string>::iterator quastion;
};

#endif
