#include "modul.h"

#include <string>
#include <vector>

class RepositoryModul : public FileModul{
public:
    typedef std::string string;

    RepositoryModul( gloox::Client *cl, const string &fileName );

    virtual ~RepositoryModul(){}

    virtual bool Message( gloox::MUCRoom* room
			  , const std::string &normal
			  , const std::string &upper
			  , const gloox::JID &from
			  ,  bool priv );

protected:

    typedef std::vector< string > ContentList;

    std::string GetRepositoryList( const string &distribution ) const;

private:
    inline bool IsDistributionName( const string &name ) const;
};

bool RepositoryModul::IsDistributionName( const string &name ) const { 
    return ( name.substr( 0, 2 ) == "::" );
}


