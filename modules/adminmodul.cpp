#include "adminmodul.h"
#include "../analyzemsg.h"

#include <gloox/mucroom.h>
#include <gloox/client.h>

AdminModul::AdminModul( gloox::Client *cl, std::list< std::string > &rootJids)
    : RootModul( cl, rootJids ){
    AddMode( "!KICK", "Выкинуть пользователя из конференции : !kick username" );
    AddMode( "!BAN", "Запретить входить пользователю в конференцию : !ban username" );
    AddMode( "!SAY", "Отправить сообщение от имени бота : !say <jid> текст сообщения" );
    AddMode( "!SETROLE", "Установить роль пользователя в данной конференции : !setrole РОЛЬ username\n"
	     "Доступные роли :\n"
	     "NONE - без роли\n"
	     "GUEST - гость\n"
	     "VISIROT - посетитель\n"
	     "MODERATOR - модератор");
    AddMode( "!SETAFF", "Установить права пользователя в данной конференции : !setaff ПРАВА username\n"
	     "Права :\n"
	     "NONE - отсутствуют\n"
	     "OUTCAST - запрещен вход\n"
	     "MEMBER - член\n"
	     "OWNER - владелец\n"
	     "ADMIN - администратор");
    AddMode( "!SUBJECT", "Установить тему в конференции : !subject тема" );
    AddMode( "!STATUS", "Установить статус бота : !status тип статус\n"
	     "Типы :\n"
	     "online, chat, away, dnd, xa" );
}

bool AdminModul::Message( gloox::MUCRoom* room
			  , const std::string &normal
			  , const std::string &upper
			  , const gloox::JID &from
			  ,  bool priv ){

    const int MSG_LENGTH = numberOfWords( normal );

    if( !IsHaveMode( getWord( upper, 0 ) ) )
	return 0;
    
    if( !IsRoot( from.full() ) ){
	NotPermissed( from, room, priv );
	return 0;
    }

    if( MSG_LENGTH < 2 ){
	LowLength( room, from, priv );
	return 0;
    }

    const std::string firstWord    = getWord( upper, 0 );
    const std::string cmd          = getWordsFrom( normal, 1 );
    const int         MODES_NUMBER = 5;
    const std::string modes[]      = { "!SAY", "!KICK", "!BAN", "!SUBJECT", "!STATUS" };
    bool              isHave       = 0;
    for( int i = 0; i < MODES_NUMBER; ++i )
	if( modes[i] == firstWord ){
	    switch( i ){
	    case 0 :
		Say(  room, cmd );
		break;
	    case 1 :
		Kick( room, cmd );
		break;
	    case 2 :
		Ban(  room, cmd );
		break;
	    case 3 :
		room->setSubject( cmd );
		break;
	    case 4 :
		if( GetPresence( getWord( upper, 1 ) ) != gloox::Presence::Error ){
		    const std::string status = ( MSG_LENGTH > 2 ? getWordsFrom( normal, 2 ) : "" );
		    room->setPresence( GetPresence( getWord( upper, 1 ) ),
				       status );
		}else 
	            room->setPresence( gloox::Presence::Chat, cmd );
		break;
	    default : break;
	    }
	    isHave = 1;
	}
    if( isHave )
	return 1;
    
    if( MSG_LENGTH < 3 ){
	LowLength( room, from.resource(), priv );
	return 0;
    }

    if( firstWord == "!SETROLE" )
	SetRole( room, getWord( upper, 1 ), getWordsFrom( normal, 2 ) );
    else if( firstWord == "!SETAFF" )
	SetAff( room, getWord( upper, 1 ), getWordsFrom( normal, 2 ) );
    return 1;
}

void AdminModul::Ban(gloox::MUCRoom *room, const std::string &user){
    room->ban(user, "Так велел мой повелитель :P");
}

void AdminModul::Kick(gloox::MUCRoom *room, const std::string &user){
    room->kick( user );
}

void AdminModul::Say(gloox::MUCRoom *room, const std::string &msg){
    if( numberOfWords( msg ) < 2 ){
	room->send( msg );
	return;
    }

    std::string jid = getWord( msg, 0 );
    std::string::size_type pos = 0;
    if( ( ( pos = jid.find( '@' ) ) == std::string::npos )
	|| ( ( pos = jid.find( '.', pos ) ) == std::string::npos )
	|| ( pos >= jid.size() ) )
	room->send( msg );
    else
	Send( gloox::JID( jid ), getWordsFrom( msg, 1 ) );
}

void AdminModul::SetRole( gloox::MUCRoom *room, const std::string &role, const std::string &user ){
    const int ROLES     = 4;
    std::string roles[] = { "NONE", "GUEST", "VISITOR", "MODERATOR" };
    for( int i = 0; i < ROLES; ++i )
	if( role == roles[i] ){
	    gloox::MUCRoomRole to = gloox::RoleNone;
	    switch( i ){
	    case 0 : to = gloox::RoleNone;        break;
	    case 1 : to = gloox::RoleVisitor;     break;
	    case 2 : to = gloox::RoleParticipant; break;
	    case 3 : to = gloox::RoleModerator;   break;
	    }
	    room->setRole( user, to );
	    break;
	}
}

void AdminModul::SetAff( gloox::MUCRoom *room, const std::string &aff, const std::string &user ){
    const int AFFLES = 5;
    const std::string affles[] = { "NONE", "OUTCAST", "MEMBER", "OWNER", "ADMIN" };
    for( int i = 0; i < AFFLES; ++i )
	if( aff == affles[i] ){
	    gloox::MUCRoomAffiliation to = gloox::AffiliationNone;
	    switch( i ){
	    case 0 : to = gloox::AffiliationNone;    break;
	    case 1 : to = gloox::AffiliationOutcast; break;
	    case 2 : to = gloox::AffiliationMember;  break;
	    case 3 : to = gloox::AffiliationOwner;   break;
	    case 4 : to = gloox::AffiliationAdmin;   break;
	    }
	    room->setAffiliation( user, to, "Так было велено мне" );
	    break;
	}
}

gloox::Presence::PresenceType AdminModul::GetPresence( const std::string &from ) const {
    const int TYPES_NUMBER    = 5;
    const std::string types[] = { "ONLINE", "CHAT", "AWAY", "DND", "XA" };
    gloox::Presence::PresenceType cur = gloox::Presence::Error;
    for( int i = 0; i < TYPES_NUMBER; ++i )
	if( types[ i ] == from )
	    switch( i ){
	    case 0 : cur = gloox::Presence::Available; break;
	    case 1 : cur = gloox::Presence::Chat; break;
	    case 2 : cur = gloox::Presence::Away; break;
	    case 3 : cur = gloox::Presence::DND; break;
	    case 4 : cur = gloox::Presence::XA; break;
	    }
    return cur;
}

/*AffiliationNone 	No affiliation with the room.
AffiliationOutcast 	The user has been banned from the room.
AffiliationMember 	The user is a member of the room.
AffiliationOwner 	The user is a room owner.
AffiliationAdmin 	The user is a room admin. 
*/
//Not work, don't know why
/*
bool AdminModul::IsGoodRole( gloox::MUCRoom *room, const gloox::JID &from ){
    bool isGood = ( room->role() == gloox::RoleModerator || room->role() == gloox::RoleParticipant );
    if( !isGood )
	Send( from, "У меня недостаточно прав" );
    return isGood;
}
*/
