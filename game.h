#ifndef GAME_H
#define GAME_H

#include <string>
#include <map>

class Game{
public:
    virtual             ~Game(){
    }
    virtual bool        IsAnswer( const std::string &answer ) const = 0;
    virtual std::string GetAnswers()  const = 0;
    virtual std::string GetQuastion() const = 0;
    virtual void        NextQuastion( const std::string &user, const int points = 1 ) = 0;
    virtual std::string GetUserStat( const std::string &user) const {
	std::map< std::string, int >::const_iterator tmp = users.find( user );
	return ( tmp == users.end() ) ? "" : DigitToString( tmp->second );
    }
    virtual std::string GetFullStat() const {
	std::string result;
	for( std::map< std::string, int >::const_iterator i = users.begin(); i != users.end(); ++i )
	    result += i->first + " : " + DigitToString( i->second ) + '\n';
	return result;
    }
protected:

    static  std::string DigitToString( int digit ){
	if( !digit )
	    return "0";
	digit = ( digit < 0 ) ? -1 * digit : digit;
	char digits[] = "0123456789";
	std::string result;
	while( digit ){
	    result = digits[ digit % 10 ] + result;
	    digit /= 10;
	}
	return result;
    }

    void                AddPoints( const std::string &user, const int points = 1){
	std::map< std::string, int >::iterator tmp = users.find( user );
	if( tmp != users.end() )
	    tmp->second += points;
	else
	    users.insert( std::make_pair( user, points ) );
    }

    std::map< std::string, int > users;
};

#endif
