#ifndef ALIASMODUL_H
#define ALIASMODUL_H

#include "modul.h"

#include <map>
#include <string>

class AliasModul : public FileModul{
public:
    AliasModul( gloox::Client *cl, const std::string &fileName );
    virtual      ~AliasModul(){}
    virtual bool Message( gloox::MUCRoom* room
			  , const std::string &normal
			  , const std::string &upper
			  , const gloox::JID &from
			  , bool priv );
    void         ReplaceAliases( const gloox::JID &jid, std::string &str ) const;
protected:
    AliasModul( const AliasModul& );
    typedef std::map< std::string, std::string > Aliases;
    std::map< std::string, Aliases >             jidsAlias;
    Aliases                                      globalAlias;
};
    
#endif
