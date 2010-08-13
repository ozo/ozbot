#ifndef BOT_H
#define BOT_H

#include "modules/mucmodul.h"

#include <gloox/client.h>
#include <gloox/eventhandler.h>
#include <gloox/connectionlistener.h>

#include <pthread.h>

#include <queue>
#include <ctime>

class Bot : public gloox::ConnectionListener, gloox::EventHandler {
public:
    Bot(  const std::string jid_
	  , const std::string pswd
	  , const std::list< std::string > &mucJids
	  , const std::list< std::string > &rootJid
	  , const std::list< std::string > &mods );
    ~Bot();
    void                      Start();
    bool                      ConnectionError();
    virtual void              onConnect();
    virtual void              onDisconnect( gloox::ConnectionError e ){}
    virtual bool              onTLSConnect(const gloox::CertInfo&){ return 1; }
protected:
    gloox::Client             *j;
    std::string               jid;
    std::string               password;
    std::list< std::string >  mucs;
    std::list< std::string >  roots;
    std::list< std::string >  modes;
    MUCModul                  *mucModul;
private:
    pthread_t                 thread;
    std::time_t               pingPongRequestTime;
    bool                      badConnection;

    static void*              ThreadMetod( void *tmp );
    void                      Ping();
    virtual void              handleEvent( const gloox::Event  &event);
};

#endif
