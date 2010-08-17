#ifndef QUIZGAME_H
#define QUIZGAME_H

#include "game.h"

#include <string>
#include <map>

class QuizGame : public Game{
public:
    QuizGame( const std::string &filename );
    virtual ~QuizGame(){
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
