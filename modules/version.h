#ifndef VERSION_H
#define VERSION_H

#include <gloox/iqhandler.h>
#include <gloox/client.h>

#include <list>
#include <queue>

namespace gloox{
    class JID;
    class Tag;
    class StanzaExtension;
}

class Version : public gloox::IqHandler{
public:
    struct version{
	gloox::JID  jid;
	std::string os;
	std::string client;
	std::string clientVersion;
    };

    class VersionHandler{
    public:
	virtual                        ~VersionHandler(){}
	virtual void                   HandleVersion( const version &v, int context ) = 0;
	virtual void                   HandleVersionError() = 0;
    };

    class Query : public gloox::StanzaExtension{
    public:
	                               Query( const gloox::Tag* tag = 0 );
	virtual                        ~Query();
	version                        Result() const;
	virtual const std::string      &filterString() const;
	virtual gloox::StanzaExtension *newInstance( const gloox::Tag* tag ) const;
	virtual gloox::Tag*            tag() const ;
	virtual gloox::StanzaExtension *clone() const;
    private:
	version v;
    };

                                       Version( gloox::Client* cl );
                                       ~Version();
    void                               query( const gloox::JID& jid, const int context = 0 );
    void                               RegisterVersionHandler( VersionHandler *vh );
    void                               RemoveVersionHandler( VersionHandler *vh );
protected:
    virtual bool                       handleIq( const gloox::IQ& iq ){ return 0; }
    virtual void                       handleIqID( const gloox::IQ &iq, int context );
    void                               SendToHandlers( const version &v, const int context );
    void                               SendErrorToHandlers();
    gloox::Client                      *client;
    std::list< VersionHandler* >       handlers;
private:
    std::queue< gloox::JID >           jids;
};

#endif
