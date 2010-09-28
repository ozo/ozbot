#ifndef QUASTIONMODUL_H
#define QUASTIONMODUL_H

#include "modul.h"

#include <list>
#include <string>
#include <istream>
#include <ostream>

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
    struct Quastion{
	std::string from;
	std::string to;
	std::string quastion;
	std::string answer;
    };
    bool ReadQuastion( std::istream &stream, Quastion &quastion );
    void WriteQuastion( std::ostream &stream, const Quastion &quastion );
    void WriteQuastions();
    bool IsHaveUser( const std::string &user );
    std::list< Quastion >           qList;
    std::list< std::string >        users;
    std::string                     qFile;
};

#endif
