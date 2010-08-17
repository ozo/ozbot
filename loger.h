#include <iostream>

class Loger{
public:
    enum PrintPriority{
	ALL = 0, TEST, DEBUG, NONE
    };
    inline        Loger( std::ostream &stream, const PrintPriority maxPrintPriority );
    inline Loger& operator<<( const PrintPriority newPriority );
    inline bool   operator<<( const std::string &msg );
protected:
    std::ostream        *outStream;
    PrintPriority       cur;
    const PrintPriority needForPrint; 
private:
    Loger( const Loger &from );
};

Loger::Loger( std::ostream &stream, const PrintPriority maxPrintPriority )
    : outStream( &stream )
    , cur( NONE )
    , needForPrint( maxPrintPriority ){
}

Loger& Loger::operator<<( const PrintPriority newPriority ){
    cur = newPriority;
    return *this;
}

bool Loger::operator<<( const std::string &msg ){
    bool returnValue = 0;
    if( outStream && cur >= needForPrint && cur != NONE ){
	*outStream << msg << '\n';
	returnValue = 1;
    }
    cur = NONE;
    return returnValue;
}

static Loger DEBUG( std::cout, Loger::DEBUG );
