#include "calcmodul.h"
#include "../analyzemsg.h"

#include <iterator>
#include <algorithm>
#include <cctype>

#include <boost/lexical_cast.hpp>

std::ostream &operator << (std::ostream &stream, const CalcModul::Digit &from ){
    const std::string SPOINT = ".";
    const char        CPOINT = '.';
    const std::string SNULL  = "0";
    const char        CNULL  = '0';
    int               begin  = 0;

    mp_exp_t pointPosition;
    std::string digit = from.value->get_str( pointPosition );

    if( digit[ 0 ] == '-' ){
	++pointPosition;
	begin = 1;
    }

    for(; pointPosition < 0; ++pointPosition )
	digit.insert( 0, SNULL );

    if( pointPosition < digit.length() ){
	if( pointPosition == begin )
	    digit.insert( pointPosition, SNULL + SPOINT );
	else
	    digit.insert( pointPosition, SPOINT );
    } else
	while( pointPosition > digit.length() )
	    digit += CNULL;

    const std::string::size_type newPointPosition = digit.find( CPOINT ); 
    if( newPointPosition != std::string::npos 
	&& newPointPosition + 10 < digit.length() )
	digit.erase( newPointPosition + 10 );
    bool isNull = 1;
    for( std::string::size_type i = 0; i < digit.length(); ++i )
	if( digit[ i ] != CPOINT && digit[ i ] != CNULL )
	    isNull = 0;

    if( isNull )
	digit = SNULL;

    stream << digit;
    return stream;
}

std::istream &operator >> (std::istream &stream, CalcModul::Digit &to ){
    stream >> *to.value;
    return stream;
}

bool CalcModul::Message( gloox::MUCRoom* room
				  , const std::string &normal
				  , const std::string &upper
				  , const gloox::JID &from
				  , bool priv ){
    if( getWord( upper, 0 )  == "!CALC" ){
	std::string result;
	if( numberOfWords( upper ) > 1 )
	    result = Answer( getWordsFrom( upper, 1 ) );
	else
	    result = "Неправильно заданы параметры : !help !calc";
	Send( room, from, result, priv );
	return 1;
    } else
	return 0;

}

bool CalcModul::IsSystemChar( const char ch ) const {
    const int CHARS_NUMBER = 4;
    const char systemChars[] = { '+', '-', '/', '*' };
    for( int i = 0; i < CHARS_NUMBER; ++i )
	if( ch == systemChars[ i ] )
	    return 1;
    return 0;
}

CalcModul::charType CalcModul::GetCharType( const char ch ) const {
    if( std::isdigit( ch ) )
	return DIGIT;
    if( IsSystemChar( ch ) )
	return SYSTEM;
    switch( ch ){
    case '.' :
	return POINT;
    case ')' :
	return RBRACKET;
    case '(' :
	return LBRACKET;
    default :
	return BAD;
    }
}

CalcModul::Digit CalcModul::GetDigit( std::string &str ) const {
    std::string tmp;
    bool isNegativeNumber = ( !str.empty() && str[ 0 ] == '-' );
    while( !str.empty() )
	if( str[ 0 ] == '-' && isNegativeNumber ){
	    isNegativeNumber = 0;
	    tmp += str[ 0 ];
	    str.erase( 0, 1 );
	} else 
	    if( std::isdigit( str[ 0 ] ) || str[ 0 ] == '.'  ){
		tmp += str[ 0 ];
		str.erase( 0, 1 );
	    } else  
		break;
    return boost::lexical_cast< Digit >( tmp );
}

void CalcModul::MakeOperation( Digits &digits
			       , Operations &symbols
			       , const char operation ) const 
    throw( ParseError,  Digit::DivisionByZero ) {

    if( !IsSystemChar( operation ) )
	throw ParseError();
    
    for( Operations::iterator it = symbols.begin(); it != symbols.end(); )
	if( *it == operation ){
	    Digits::iterator first  = digits.begin();
	    Digits::iterator second = digits.begin();
	    std::advance( first,  std::distance( symbols.begin(), it ) );
	    std::advance( second, std::distance( symbols.begin(), it ) + 1 );
	    switch( operation ){
	    case '/' :
		*first /= *second;
		break;
	    case '*' :
		*first *= *second;
		break;
	    case '+' :
		*first += *second;
		break;
	    case '-' :
		*first -= *second;
		break;
	    default :
		throw ParseError();
		break;
	    }
	    digits.erase( second );
	    it = symbols.erase( it );
	} else
	    ++it;
}

std::string CalcModul::Calculate( std::string str ) const
    throw( ParseError, Digit::DivisionByZero ) {
    std::list< Digit > digits;
    std::list< char > symbols;
    for( int i = 0; !str.empty(); ++i )
	if( i % 2 ){
	    symbols.push_back( str[0] );
	    str.erase( 0, 1 );
	} else
	    digits.push_back( GetDigit( str ) );

    const int OPERATIONS_NUMBER  = 4;
    const char operations[] = { '/', '*', '-', '+' };
    
    for( int i = 0; i < OPERATIONS_NUMBER; ++i )
	MakeOperation( digits, symbols, operations[ i ] );

    if( digits.size() > 1 )
	throw ParseError();
    else
	return boost::lexical_cast< std::string >( *digits.begin() );
}

std::string CalcModul::Answer( std::string str ){
    while( str.find( ' ' ) != std::string::npos )
	str.erase( str.find( ' ' ), 1 );

    const std::string error = "Ошибка форматирования";
    std::string token, expression;

    if( str.find( '=' ) != std::string::npos ){
	bool isGood = 1;
	token = str.substr( 0, str.find( '=' ) );
	for( std::string::size_type i = 0; i < token.length(); ++i )
	    if( GetCharType( token[ i ] ) != BAD )
		isGood = 0;
	if( isGood ){
	    str.erase( 0, str.find( '=' ) + 1 );
	    expression = str;
	} else {
	    return error;
	}
    } else {
	expression = str;
	token = "answer";
    }

    for( Tokens::const_iterator it = tokens->begin(); it != tokens->end(); ++it )
	while( expression.find( it->first ) != std::string::npos ){
	    std::string::size_type pos = expression.find( it->first );
	    const bool leftBad  = ( pos && GetCharType( expression[ pos - 1 ] ) == DIGIT );
	    pos += it->first.length();
	    const bool rightBad = ( GetCharType( expression[ pos ] ) == DIGIT );
	    if( ( pos && leftBad ) || ( pos < expression.length() && rightBad ) )
		return error;
	    expression.replace( expression.find( it->first )
				, it->first.length()
				, it->second );
	}

    for( std::string::size_type i = 0; i < expression.length(); ++i )
	if( GetCharType( expression[ i ] ) == BAD ) 
	    return error;

    int leftBracket = 0, rightBracket = 0;

    for( unsigned int i = 0; i < expression.length(); ++i ){
	const bool end = ( i == ( expression.length() - 1 ) );

	charType left = DIGIT;
	if( i )
	    left = GetCharType( expression[ i - 1 ] );
	bool leftBad = 0;

	charType right = DIGIT;
	if( !end  )
	    right = GetCharType( expression[ i + 1 ] );
	bool rightBad = 0;

	switch( expression[ i ] ){
	case '(' :
	    leftBracket++;
	    leftBad  = ( i && ( left != LBRACKET && left != SYSTEM ) );
	    rightBad = ( end || ( right != LBRACKET 
				  && right != DIGIT && expression[ i + 1 ] != '-' ) );
	    break;
	case ')' :
	    rightBracket++;
	    leftBad = ( !i || ( left != RBRACKET && left != DIGIT ) );
	    rightBad = ( !end && ( right != RBRACKET && right != DIGIT && right != SYSTEM ) );
	    if( rightBracket - leftBracket > 0 )
		return error;
	    break;
	case '.' :
	    leftBad  = ( !i  || left  != DIGIT );
	    rightBad = ( end || right != DIGIT );
	    break;
	case '-' :
	    leftBad  = ( left != DIGIT && left != RBRACKET );
	    rightBad = ( end || ( right != DIGIT && right != LBRACKET ) );
	    break;
	case '+' :
	case '/' :
	case '*' :
	    leftBad  = ( !i || ( left != DIGIT && left != RBRACKET ) );
	    rightBad = ( end || ( right != DIGIT 
				  && right != LBRACKET 
				  && expression[ i + 1 ] != '-' ) );
	    break;
	default : break;
	}
	if( leftBad || rightBad )
	    return error;
    }

    if( leftBracket != rightBracket )
	return error;

    try{
	while( 1 ){
	    std::string::size_type lBracketMax = std::string::npos;
	    int maxNumber = 0;
	    int curNumber = 0;
	    for( std::string::size_type i = 0; i < expression.length(); ++i ){
		if( expression[ i ] == '(' ){
		    curNumber++;
		    if( curNumber > maxNumber ){
			maxNumber = curNumber;
			lBracketMax = i;
		    }
		} else if( expression[ i ] == ')' )
		    curNumber--;
	    }
	    if( lBracketMax < std::string::npos ){
		std::string::size_type bracketNext = expression.find( ')', lBracketMax );
		expression.replace( lBracketMax
				    , bracketNext - lBracketMax + 1
				    , Calculate( expression.substr( lBracketMax + 1
								    , bracketNext 
								    - lBracketMax 
								    - 1 
								    ) 
						 ) 
				    );
	    } else {
		expression = Calculate( expression );
		break;
	    }
	}
    } catch( Digit::DivisionByZero ) { 
	expression = "42";
    } catch( ParseError ){
	return "Ошибка анализа выражения";
    } catch( boost::bad_lexical_cast ){
	return "Ошибка форматирования( выявлена на этапе вычисления )";
    }

    if( token != "answer" )
	AddToken( token, expression );

    return expression;
} 

void CalcModul::AddToken( const std::string &token, const std::string &value ){
    if( HaveToken( token ) ){
	Tokens::iterator it = std::find_if( tokens->begin()
					    , tokens->end()
					    , FindStringInPairs( token ) );
	it->second = value;
    } else {
	tokens->push_back( std::make_pair( token, value ) );
	std::sort( tokens->begin(), tokens->end(), StringLengthGreater() );
    }
}
