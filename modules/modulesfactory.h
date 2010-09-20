#ifndef MODULESFACTORY_H
#define MODULESFACTORY_H

#include "modul.h"
#include "../analyzemsg.h"

#include <string>
#include <map>
#include <list>

class ModulesFactory{
public:
    typedef Modul* ( *FactoryMetod )( const std::string& );
    // source is : "module_name parameter"
    ModulesFactory( ){}
    Modul*                   InitModule( const std::string& source );
    void                     AddModule( const std::string& name, FactoryMetod metod );
    std::list< std::string > ModulesList( );
protected:
    // Module_name and ptr to init function
    std::map< std::string, FactoryMetod > modules;
private:
    ModulesFactory( ModulesFactory& );
};

static ModulesFactory MUCModulesFactory;

#endif
