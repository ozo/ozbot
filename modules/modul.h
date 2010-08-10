#ifndef MODUL_H
#define MODUL_H

#include <map>
#include <string>
#include <list>
#include <algorithm>

namespace gloox{
    class MUCRoom;
    class Client;
    class JID;
}

class Modul{
public:

    Modul( gloox::Client *cl)
	: client( cl ){
    }

    virtual                          ~Modul(){
	client = 0;
    }
    
    virtual bool                     Message( gloox::MUCRoom* room
					      , const std::string &normal
					      , const std::string &upper
					      , const gloox::JID &from
					      ,  bool priv ) = 0;

    virtual std::string              GetModeInfo( const std::string &mode );
    virtual std::list< std::string > GetModes() const;

    virtual bool IsHaveMode( const std::string &mode ) const {
	return ( modes.find( mode ) != modes.end() ); 
    }
protected:
    inline void AddMode( const std::string &mode, const std::string &info = "" );
    void Send( const gloox::JID &to
	       , const std::string &msg
	       , const std::string theme = "" ) const;
    void Send( gloox::MUCRoom *room
	       , const gloox::JID &to
	       , const std::string &msg
	       , bool priv ) const;
    void LowLength( gloox::MUCRoom *muc, const gloox::JID &from, const bool priv = 0 ) const ;
	
    std::multimap< std::string, std::string > modes;
    gloox::Client *client;
};

class RootModul : public Modul{
public :
    typedef std::string         string;
    typedef std::list< string > RootList;

    RootModul( gloox::Client *cl, const RootList &from );
    virtual ~RootModul(){
	RemoveRoots( *roots );
    }
    inline bool IsRoot( const string &name ) const;
    void        NotPermissed( const gloox::JID &user, gloox::MUCRoom *room, const bool priv );
protected:
    RootList     *roots;
private  :

    typedef std::list< std::pair< RootList, unsigned int > > Roots;
    void                            InitRoots( const RootList &roots );
    void                            RemoveRoots( const RootModul::RootList &roots );
    static Roots                           rootsVersions;
};

bool RootModul::IsRoot( const RootModul::string &name ) const {
    return std::find( roots->begin(), roots->end(), name ) != roots->end();
}

class FileModul : public Modul{
public:
    class   FileNotFound{};
    typedef std::string string;
    typedef std::list< string > StringList;
    
    FileModul( gloox::Client *cl, const string &fileName );
protected:
    void        EraseComments();
    StringList  content;
private:
    static void EraseComment( string &str);
};

void Modul::AddMode( const std::string &mode, const std::string &info ){
    modes.insert( std::make_pair( mode, info ) );
}

#endif
