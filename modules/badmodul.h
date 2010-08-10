#ifndef BADMODUL_H
#define BADMODUL_H

#include "modul.h"

#include "../analyzemsg.h"

#include <map>

class BadModul : public FileModul{
public:
    BadModul( gloox::Client *cl, const std::string &fileName );

    virtual bool Message( gloox::MUCRoom* room
			  , const std::string &normal
			  , const std::string &upper
			  , const gloox::JID &from
			  ,  bool priv );

    virtual std::list< std::string > GetModes() const {
	return std::list< std::string >();
    }

protected:
    enum Man{
	NEW, OLD, BAD
    };
    typedef std::map< std::string, int > PupilsCount;

    PupilsCount badBoys;

    void Send( const gloox::JID &from
	       , gloox::MUCRoom *room
	       , Man mode
	       , const bool priv ) const;

};

#endif
