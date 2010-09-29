#include "infomodul.h"
#include "../analyzemsg.h"

#include <gloox/mucroom.h>
#include <gloox/client.h>

#include <sstream>
#include <algorithm>

InfoModul::InfoModul( gloox::Client *cl )
    : Modul( cl )
    , vcardMng( cl )
    , version( cl ){
    version.RegisterVersionHandler( this );
    AddMode( "!VCARD",  "Вывести в чат конференции vcard пользователя : !vcard username" );
    AddMode( "!CLIENT", "Вывести в чат конференции клиент пользователя : !client username" );
    AddMode( "!PING", "Получить ping от бота : !ping <username>" );
    AddMode( "!OS", "Получить название ОС пользователя : !os username" );
}

bool InfoModul::Message( gloox::MUCRoom *room
		       , const std::string &normal
 		       , const std::string &upper
		       , const gloox::JID &from
		       , bool priv ){
    const std::string FIRST_WORD = getWord( upper, 0 );
    const int         MSG_LENGTH = numberOfWords( normal );

    if( !IsHaveMode( FIRST_WORD ) )
	return 0;

    Request newRequest = { room, priv, from };

    if( FIRST_WORD == "!PING" ){
	requests.push_back( newRequest );
	struct timeval curTime;
	struct timezone tz;
	gettimeofday( &curTime, &tz );
	pingTimes.push( curTime  );
	if(  MSG_LENGTH > 1 ){
	    const std::string jid = room->name() 
		+ '@' 
		+ room->service( ) 
		+ '/' 
		+ getWordsFrom( normal, 1 );
	    client->xmppPing( gloox::JID(  jid ), this );
	} else
	    client->xmppPing( from, this );
	return 1;
    }

    std::string user;
    if( MSG_LENGTH > 1 )
	user = room->name() 
	    + '@' + room->service() 
	    + '/' + getWordsFrom( normal, 1 );
    else
	user = from.full( );

    newRequest.to = user;
    requests.push_back( newRequest );
    if( FIRST_WORD == "!OS" )
	version.query( user, 0 );
    else if( FIRST_WORD == "!VCARD" )
	GetVcard( user );
    else if( FIRST_WORD == "!CLIENT" )
	version.query( user, 1 );
    else
	requests.pop_back( );
    return 1;
}
    
void InfoModul::handleVCard( const gloox::JID& jid, const gloox::VCard* vcard ){
    std::string msg;
    if( !vcard )
	msg = "Карточка пуста";
    else{
	msg += '\n';
	msg += vcard->formattedname().empty() ? "" : ( " Имя: " + vcard->formattedname() + '\n' );
	msg += vcard->nickname().empty() ? "" : ( " Ник: " + vcard->nickname() + '\n' );
	msg += vcard->bday().empty() ? "" : ( " Дата рождения:" + vcard->bday() + '\n' );
	msg += vcard->url().empty() ? "" : ( " URL: " + vcard->url() + '\n' );
	msg += vcard->desc().empty() ? "" : ( " Обо мне :\n" + vcard->desc() );
    }
    Send( msg );
}

void InfoModul::handleEvent( const gloox::Event &event){
    struct timeval curTime;
    struct timezone tz;
    gettimeofday( &curTime, &tz );
    std::string msg;
    if( !pingTimes.empty() ){
	if( event.eventType() == gloox::Event::PingPong ){
	    msg = "Ответ на !ping был получен через : ";
	    int secs  = curTime.tv_sec - pingTimes.front().tv_sec;
	    int usecs = curTime.tv_usec - pingTimes.front().tv_usec;
	    pingTimes.pop();
	    if( usecs < 0 ){
		secs--;
		usecs *= -1;
	    }
	    std::stringstream buffer;
	    buffer << secs << '.' << usecs;
	    std::string time;
	    buffer >> time;
	    msg += time + " сек.";
	} else if( event.eventType() == gloox::Event::PingError ){
	    pingTimes.pop( );
	    msg = "Ошибка получения результата на !ping";
	}
    } else
	msg = "Ошибка : неправильная работа со временем";
    Send( msg );
}

void InfoModul::Send( const std::string &msg ){
    if( requests.empty() )
	return;
    std::list< Request >::iterator first = requests.begin( );
    Modul::Send( first->room
		 , first->from
		 , msg
		 , first->priv );

    requests.erase( first );

}

void InfoModul::Send( const std::string &msg, const gloox::JID &to ){
    if( !to ){
	Send( msg );
	return;
    }
    for( std::list< Request >::iterator i = requests.begin( ); i != requests.end( ); ++i )
	if( i->to == to ){
	    Modul::Send( i->room, i->from, msg, i->priv );
	    requests.erase( i );
	    return;
	}
    Send( msg );
}

void InfoModul::HandleVersion(const Version::version &v, int context ){
    std::string msg;
    const std::string user = v.jid.resource();
    if( !context ){
	if( v.os.empty() ){
	    GetError();
	    return;
	}
	msg = "операционная система пользователя " + user + " : " + v.os;
    }else {
	if( v.client.empty() ){
	    GetError();
	    return;
	}
	msg = "клиент пользователя " + user + " : " + v.client;
	if( !v.clientVersion.empty() )
	    msg += ", версия : " + v.clientVersion;
    }
    Send( msg );
}
