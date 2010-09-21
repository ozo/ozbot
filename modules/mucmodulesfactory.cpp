#include "mucmodulesfactory.h"

MUCModulesFactory* MUCModulesFactory::factory = 0;

MUCModulesFactory* MUCModulesFactory::GetFactory( ){
    if( !factory )
	factory = new MUCModulesFactory( );
    return factory;
}

void MUCModulesFactory::DestroyFactory( ){
    delete factory;
    factory = 0;
}

MUCModulesFactory::MUCModulesFactory( ){
    AddModule( "ADMIN", InitAdminModule );
    AddModule( "ALIAS", InitAliasModule );
    AddModule( "BAD", InitBadModule );
    AddModule( "CALC", InitCalcModule );
    AddModule( "GAME", InitGameModule );
    AddModule( "INFO", InitInfoModule );
    AddModule( "LOG", InitLogModule );
    AddModule( "PARSER", InitParseModule );
    AddModule( "QUASTIONS", InitQuastionModule );
    AddModule( "REPO", InitRepositoryModule );
}
