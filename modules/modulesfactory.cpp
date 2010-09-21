#include "modulesfactory.h"

#include "adminmodul.h"
#include "aliasmodul.h"
#include "badmodul.h"
#include "calcmodul.h"
#include "gamemodul.h"
#include "infomodul.h"
#include "logmodul.h"
#include "parsemodul.h"
#include "quastionmodul.h"
#include "repositorymodul.h"
#include "timemodul.h"

#include <iostream>

Modul*  ModulesFactory::InitModule( const std::string& source ){
    std::string moduleName = getWord( source, 0 );
    std::string parameter;
    if( numberOfWords( source) )
	parameter  = getWordsFrom( source, 1 );

    if( modules.find( moduleName ) != modules.end( ) )
	return modules.find( moduleName )->second( parameter, client );
    return 0;
}

void ModulesFactory::AddModule( const std::string& name, FactoryMetod metod ){
    modules.insert( std::make_pair( name, metod ) );
}

std::list< std::string > ModulesFactory::ModulesList( ){
    std::list< std::string > result;
    for( std::map< std::string, FactoryMetod >::iterator i = modules.begin( )
	     ; i != modules.end( ); ++i )
	result.push_back( i->first );
    return result;
}

Modul* InitAdminModule( const std::string&, gloox::Client *cl ){
    return new AdminModul( cl );
}

Modul* InitAliasModule( const std::string &fileName, gloox::Client *cl ){
    return new AliasModul( cl, fileName );
}

Modul* InitBadModule( const std::string &fileName, gloox::Client *cl ){
    return new BadModul( cl, fileName );
}

Modul* InitCalcModule( const std::string&, gloox::Client *cl ){
    return new CalcModul( cl );
}

Modul* InitGameModule( const std::string&, gloox::Client *cl ){
    return new GameModul( cl );
}

Modul* InitInfoModule( const std::string&, gloox::Client *cl ){
    return new InfoModul( cl );
}

Modul* InitLogModule( const std::string&, gloox::Client *cl ){
    return new LogModul( cl );
}

Modul* InitParseModule( const std::string&, gloox::Client *cl ){
    return new ParseModul( cl );
}

Modul* InitQuastionModule( const std::string &fileName, gloox::Client *cl ){
    return new QuastionModul( cl, fileName );
}

Modul* InitRepositoryModule( const std::string &fileName, gloox::Client *cl ){
    return new RepositoryModul( cl, fileName );
}

Modul* InitTimeModule( const std::string &, gloox::Client *cl ){
    return new TimeModul( cl );
}
