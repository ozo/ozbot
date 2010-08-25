#ifndef TIMEIQ_H
#define TIMEIQ_H

#include <gloox/iqhandler.h>
#include <gloox/client.h>

#include <list>
#include <queue>
#include <ctime>

namespace gloox{
    class JID;
    class Tag;
    class StanzaExtension;
}

class TimeIq : public gloox::IqHandler{
public:
    struct time{
	gloox::JID  jid;
	std::string time;
    };

    class TimeHandler{
    public:
	virtual                        ~TimeHandler(){}
	virtual void                   HandleTime( const time &t, int context ) = 0;
	virtual void                   HandleTimeError() = 0;
    };

    class TimeQuery : public gloox::StanzaExtension{
    public:
	                               TimeQuery( const gloox::Tag* tag = 0 );
	virtual                        ~TimeQuery(){}
	inline  time                   Result() const { return t; }
	virtual const std::string      &filterString() const;
	virtual gloox::StanzaExtension *newInstance( const gloox::Tag* tag ) const;
	virtual gloox::Tag*            tag() const ;
	virtual gloox::StanzaExtension *clone() const;
    private:
	time t;
    };

                                       TimeIq( gloox::Client* cl );
                                       ~TimeIq();
    void                               Query( const gloox::JID& jid, const int context = 0 );
    void                               RegisterTimeHandler( TimeHandler *vh );
    void                               RemoveTimeHandler( TimeHandler *vh );
protected:
    virtual bool                       handleIq( const gloox::IQ& iq );
    virtual void                       handleIqID( const gloox::IQ &iq, int context );
    void                               SendToHandlers( const time &t, const int context );
    void                               SendErrorToHandlers();
    gloox::Client                      *client;
    std::list< TimeHandler* >          handlers;
private:
    std::pair< gloox::JID, std::time_t > *curJID;
    unsigned int                         quarysCount;
};

#endif
