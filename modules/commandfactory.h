#ifndef COMMANDFACTORY_H
#define COMMANDFACTORY_H

#include <string>
#include <map>

template< typename Module, typename ReturnValue = bool, typename ParameterValue = const std::string& >
class CommandFactory{
public:
    typedef ReturnValue ( Module::*CommandPtr )( gloox::MUCRoom*, ParameterValue );
    CommandFactory( ){}
    inline  CommandPtr  GetPtr( const std::string &moduleName ){
	if( commands.find( moduleName ) != commands.end( ) )
	    return commands.find( moduleName )->second;
	return 0;
    }
    inline  void        AddCommand( const std::string &moduleName, CommandPtr metod ){
	if( commands.find( moduleName ) == commands.end( ) )
	    commands.insert( std::make_pair( moduleName, metod ) );
    }
protected:
    CommandFactory( CommandFactory& );
    std::map< std::string, CommandPtr > commands;
};

#endif
