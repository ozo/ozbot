#include "modulesfactory.h"

Modul*  ModulesFactory::InitModule( const std::string& source ){
    std::string moduleName;
    std::string parameter;
    if( numberOfWords( source ) < 2 )
	moduleName = source;
    else {
	moduleName = getWord( source, 0 );
	parameter  = getWordsFrom( source, 1 );
    }
    if( modules.find( moduleName ) != modules.end( ) )
	return modules.find( moduleName )->second( parameter );
    return 0;
}

void ModulesFactory::AddModule( const std::string& name, FactoryMetod metod ){
    modules.insert( std::pair< std::string, FactoryMetod >( name, metod ) );
}

std::list< std::string > ModulesFactory::ModulesList( ){
    std::list< std::string > result;
    for( std::map< std::string, FactoryMetod >::iterator i = modules.begin( )
	     ; i != modules.end( ); ++i )
	result.push_back( i->first );
    return result;
}
