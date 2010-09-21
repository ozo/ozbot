#ifndef MUCMODULESFACTORY_H
#define MUCMODULESFACTORY_H

#include "modulesfactory.h"

class MUCModulesFactory : public ModulesFactory{
public:
    static MUCModulesFactory *GetFactory( );
    static void               DestroyFactory( );
private:
    static MUCModulesFactory *factory;
    MUCModulesFactory( );
};

#endif
