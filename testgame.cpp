#include "testgame.h"

#include <fstream>
#include <algorithm>
#include "analyzemsg.h"

TestGame::TestGame( const std::string &filename ){
    std::string str;
    std::list< std::string > file;
    std::ifstream in( filename.c_str() );
    if( in )
	while( std::getline( in, str ) )
	    file.push_back( str );
    in.close();

    Quastion tmpQ;
    for( std::list< std::string >::const_iterator i = file.begin(); i != file.end(); ++i ){
	if( (*i)[0] == 'Q' ){
	    if( !tmpQ.answers.empty() )
		quastions.push_back( tmpQ );
	    tmpQ.answers.clear();
	    tmpQ.quastion = *i;
	}
	if( (*i)[0] == 'A' || (*i)[0] == 'N' )
	    tmpQ.answers.push_back( *i );
    }

    if( !tmpQ.answers.empty() )
	quastions.push_back( tmpQ );

    if( quastions.empty() )
	quastions.push_back( tmpQ );
    cur = quastions.begin();
}

bool TestGame::IsAnswer( const std::string &number ) const {
    std::string tmp;
    std::list< std::string >::const_iterator answer = cur->answers.begin();
    for(; answer != cur->answers.end(); ++answer ){
	tmp = toUpper( answer->substr( 2 ) );
	if( (*answer)[0] == 'A' && number == tmp )
	    return 1;
    }
    return 0;
}

std::string TestGame::GetAnswers() const {
    std::string result;
    std::string tmp;
    for( std::list< std::string >::iterator i = cur->answers.begin(); i != cur->answers.end(); ++i ){
	tmp = toUpper( i->substr( 2 ) );
	result +=  tmp + ' ';
    }
     return result;
}


std::string TestGame::GetQuastion() const {
    std::string result = cur->quastion.substr( 1 ) + '\n';
    for( std::list< std::string >::iterator i = cur->answers.begin(); i != cur->answers.end(); ++i )
	result +=  i->substr( 2 ) + '\n';
    return result;
}

void TestGame::NextQuastion( const std::string &user = "", const int points = 1 ){
    ++cur;
    if( cur == quastions.end() )
	cur = quastions.begin();
    
    if( !user.empty() )
	AddPoints( user, points );
}
