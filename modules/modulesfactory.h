#ifndef MODULESFACTORY_H
#define MODULESFACTORY_H

#include "modul.h"
#include "../analyzemsg.h"

#include <string>
#include <list>

namespace gloox{
    class Client;
}

class ModulesFactory{
public:
    typedef Modul* ( *FactoryMetod )( const std::string&, gloox::Client* );
    // source is : "module_name parameter"
    ModulesFactory( ){}
    virtual ~ModulesFactory( ){}
    inline void              SetClient( gloox::Client *cl );
    inline void              UnSetClient( );
    Modul*                   InitModule( const std::string& source );
    void                     AddModule( const std::string& name, FactoryMetod metod );
    std::list< std::string > ModulesList( );
protected:
    // Module_name and ptr to init function
    std::map< std::string, FactoryMetod > modules;
    gloox::Client                         *client;
private:
    ModulesFactory( ModulesFactory& );
};

void ModulesFactory::SetClient( gloox::Client *cl ){
    client = cl;
}

void ModulesFactory::UnSetClient( ){
    client = 0;
}

Modul* InitAdminModule( const std::string&, gloox::Client *cl );
Modul* InitAliasModule( const std::string &fileName, gloox::Client *cl );
Modul* InitBadModule( const std::string &fileName, gloox::Client *cl );
Modul* InitCalcModule( const std::string&, gloox::Client *cl );
Modul* InitGameModule( const std::string&, gloox::Client *cl );
Modul* InitInfoModule( const std::string&, gloox::Client *cl );
Modul* InitLogModule( const std::string&, gloox::Client *cl );
Modul* InitParseModule( const std::string&, gloox::Client *cl );
Modul* InitQuastionModule( const std::string &fileName, gloox::Client *cl );
Modul* InitRepositoryModule( const std::string &fileName, gloox::Client *cl );
Modul* InitTimeModule( const std::string &, gloox::Client *cl );

#endif
