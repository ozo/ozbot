#ifndef CALCMODUL_H
#define CALCMODUL_H

#include <iterator>
#include <vector>
#include <functional>
#include <memory>
#include <algorithm>

#include <gmpxx.h>

#include "modul.h"

class CalcModul : public Modul {
public:
    class Digit{
    public:
	class DivisionByZero{};
	
	Digit( const Digit &from )
	    : value( new mpf_class( *from.value ) ){

	}
	
	Digit()
	    : value( new mpf_class( "0",  512 ) ){
	}
	
	Digit( const mpf_class newValue ){
	    *value = newValue;
	}

	void operator+=( const Digit &right ){
	    *value += *right.value;
	}
	void operator-=( const Digit &right ){
	    *value -= *right.value;
	}
	void operator*=( const Digit &right ){
	    *value *= *right.value;
	}
	void operator/=( const Digit &right ) throw( DivisionByZero ){
	    if( *right.value == 0 )
		throw DivisionByZero();
	    *value /= *right.value;
	}
	void operator=(const Digit &right ){
	    *value = *right.value;
	}

	friend std::ostream &operator << (std::ostream &stream, const Digit &from );
	friend std::istream &operator >> (std::istream &stream, Digit &to );
	
	operator mpf_class() const {
	    return *value;
	}

    protected:
	std::auto_ptr< mpf_class > value;
    };

    CalcModul( gloox::Client *cl )
	: Modul( cl )
	, tokens( 0 ){
	tokens = new Tokens;
	AddMode( "!CALC", "Вычислить значение выражения : !calc выражение\n"
		          "Допустимые символы : числа, ., +, -, *, /" );
    }

    virtual ~CalcModul(){
	delete tokens;
    }

    virtual bool Message( gloox::MUCRoom* room
			  , const std::string &normal
			  , const std::string &upper
			  , const gloox::JID &from
			  , bool priv );

 protected:

    typedef std::vector< std::pair< std::string, std::string > > Tokens;
    typedef std::list< Digit > Digits;
    typedef std::list< char >  Operations;

    class ParseError{};

    class StringLengthGreater{
    public:
	typedef std::pair< std::string, std::string > StringStringPair;
	inline bool operator()( const StringStringPair &left
				, const StringStringPair &right ) const {
	    return ( left.first.length() > right.first.length() );
	}
    };

    class FindStringInPairs{
    public:
	typedef std::pair< std::string, std::string > StringStringPair;
	FindStringInPairs( const std::string &str ){
	    value = str;
	}
	inline bool operator()( const StringStringPair &pair ) const {
	    return ( pair.first == value );
	}
    protected:
	std::string value;
    };

    enum charType{
	BAD, 
	DIGIT, 
	SYSTEM, 
	POINT, 
	LBRACKET, 
	RBRACKET
    };

    inline bool HaveToken( const std::string &token ) const {
	Tokens::const_iterator it = std::find_if( tokens->begin()
					 , tokens->end()
                                         , FindStringInPairs( token ) );
	return ( it != tokens->end() );
    }

    void     AddToken( const std::string &token, const std::string &value );

    bool     IsSystemChar( const char ch ) const;
    charType GetCharType( const char ch ) const;
    Digit    GetDigit( std::string &str ) const;

    void     MakeOperation( Digits &digits
			    , Operations &symbols
			    , const char operation ) const
	throw( ParseError, Digit::DivisionByZero );

    std::string Calculate( std::string str ) const
	throw( ParseError, Digit::DivisionByZero );

    std::string Answer( std::string str );

    Tokens *tokens;
};

#endif
