#ifndef QUASTIONMODUL_H
#define QUASTIONMODUL_H

#include "modul.h"

#include <list>

class Quastion;

class QuastionModul : public FileModul{
public:
    QuastionModul( gloox::Client *cl, const std::string &quastionFile);

    virtual bool Message( gloox::MUCRoom* room
			  , const std::string &normal
			  , const std::string &upper
			  , const gloox::JID &from
			  ,  bool priv );
    void AddUser( const std::string &userFull );
    void DeleteUser( const std::string &user );
protected:
    void WriteQuastions();
    bool IsHaveUser( const std::string &user );
    std::list< Quastion >           qList;
    std::list< std::string >        users;
    std::string                     qFile;
};

#endif
