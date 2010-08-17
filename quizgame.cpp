#include "quizgame.h"

#include <fstream>

QuizGame::QuizGame(const std::string &filename ){
    std::string tmpQuastion, tmpAnswer;
    std::ifstream in( filename.c_str() );
    while( std::getline( in, tmpQuastion ) ){
	std::getline( in, tmpAnswer );
	if( !tmpQuastion.empty() && !tmpAnswer.empty() )
	    quastions.insert( std::make_pair( tmpQuastion, tmpAnswer ) );
    }
    in.close();

    if( quastions.empty() )
	quastions.insert( std::make_pair( "", "" ) );

    quastion = quastions.begin();
}

bool QuizGame::IsAnswer( const std::string &answer ) const {
    return ( quastion->second == answer );
}

std::string QuizGame::GetAnswers() const {
   return quastion->second;
}

std::string QuizGame::GetQuastion() const {
    return quastion->first;
}

void QuizGame::NextQuastion( const std::string &user = "", const int points = 1 ){
    if( !user.empty() )
	AddPoints( user, points );
    quastion++;
    if( quastion == quastions.end() )
	quastion = quastions.begin();
}
