#include "adminmodul.h"
#include "../analyzemsg.h"

#include <gloox/mucroom.h>
#include <gloox/client.h>

AdminModul::AdminModul( gloox::Client *cl )
    : RootModul( cl ){

    AddMode( "!KICK", "Выкинуть пользователя из конференции : !kick username" );
    commands.AddCommand( "!KICK", &AdminModul::Kick );

    AddMode( "!BAN", "Запретить входить пользователю в конференцию : !ban username" );
    commands.AddCommand( "!BAN", &AdminModul::Ban );

    AddMode( "!SAY", "Отправить сообщение от имени бота : !say <jid> текст сообщения" );
    commands.AddCommand( "!SAY", &AdminModul::Say );

    AddMode( "!SETROLE", "Установить роль пользователя в данной конференции : !setrole РОЛЬ username\n"
	     "Доступные роли :\n"
	     "NONE - без роли\n"
	     "GUEST - гость\n"
	     "VISIROT - посетитель\n"
	     "MODERATOR - модератор");
    commands.AddCommand( "!SETROLE", &AdminModul::SetRole );

    AddMode( "!SETAFF", "Установить права пользователя в данной конференции : !setaff ПРАВА username\n"
	     "Права :\n"
	     "NONE - отсутствуют\n"
	     "OUTCAST - запрещен вход\n"
	     "MEMBER - член\n"
	     "OWNER - владелец\n"
	     "ADMIN - администратор");
    commands.AddCommand( "!SETAFF", &AdminModul::SetAff );

    AddMode( "!SUBJECT", "Установить тему в конференции : !subject тема" );
    commands.AddCommand( "!SUBJECT", &AdminModul::SetSubject );

    AddMode( "!STATUS", "Установить статус бота : !status тип статус\n"
	     "Типы :\n"
	     "online, chat, away, dnd, xa" );
    commands.AddCommand( "!STATUS", &AdminModul::SetStatus );
}

bool AdminModul::Message( gloox::MUCRoom* room
			  , const std::string &normal
			  , const std::string &upper
			  , const gloox::JID &from
			  ,  bool priv ){

    const int MSG_LENGTH = numberOfWords( normal );

    if( !IsRoot( from.full() ) ){
	NotPermissed( from, room, priv );
	return 0;
    }

    if( MSG_LENGTH < 2 ){
	LowLength( room, from, priv );
	return 0;
    }

    const std::string commandName  = getWord( upper, 0 );
    const std::string cmd          = getWordsFrom( normal, 1 );
    if( commands.GetPtr( commandName ) )
	if( !( this->*( commands.GetPtr( commandName ) ) )( room, cmd ) ){
	    LowLength( room, from, priv );
	    return 0;
	}
    return 1;
}

bool AdminModul::SetSubject( gloox::MUCRoom *room, const std::string &cmd ){
    room->setSubject( cmd );
    return 1;
}

bool AdminModul::SetStatus( gloox::MUCRoom *room, const std::string &cmd ){
    if( GetPresence( getWord( toUpper( cmd ), 0 ) ) != gloox::Presence::Error ){
	const std::string status = ( numberOfWords( cmd ) > 1 ? getWordsFrom( cmd, 1 ) : "" );
	room->setPresence( GetPresence( getWord( toUpper( cmd ), 0 ) ), status );
    } else
	room->setPresence( gloox::Presence::Chat, cmd );
    return 1;
}

bool AdminModul::Ban(gloox::MUCRoom *room, const std::string &user){
    room->ban(user, "Так велел мой повелитель :P");
    return 1;
}

bool AdminModul::Kick(gloox::MUCRoom *room, const std::string &user){
    room->kick( user );
    return 1;
}

bool AdminModul::Say(gloox::MUCRoom *room, const std::string &msg){
    if( numberOfWords( msg ) < 2 ){
	room->send( msg );
	return 1;
    }

    std::string jid = getWord( msg, 0 );
    std::string::size_type pos = 0;
    if( ( ( pos = jid.find( '@' ) ) == std::string::npos )
	|| ( ( pos = jid.find( '.', pos ) ) == std::string::npos )
	|| ( pos >= jid.size() ) )
	room->send( msg );
    else
	Send( gloox::JID( jid ), getWordsFrom( msg, 1 ) );
    return 1;
}

bool AdminModul::SetRole( gloox::MUCRoom *room, const std::string &cmd ){
    if( numberOfWords( cmd ) < 2 )
	return 0;
    const std::string role = getWord( toUpper( cmd ), 0 );
    const std::string user = getWordsFrom( toUpper( cmd ), 1 );

    const int ROLES     = 4;
    std::string roles[] = { "NONE", "GUEST", "VISITOR", "MODERATOR" };
    bool isHave = 0;
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
	    isHave = 1;
	    break;
	}
    return isHave;
}

bool AdminModul::SetAff( gloox::MUCRoom *room, const std::string &cmd ){
    if( numberOfWords( cmd ) < 2 )
	return 0;
    const std::string aff  = getWord( toUpper( cmd ), 0 );
    const std::string user = getWordsFrom( toUpper( cmd ), 1 );

    const int AFFLES = 5;
    const std::string affles[] = { "NONE", "OUTCAST", "MEMBER", "OWNER", "ADMIN" };
    bool isHave = 0;
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
	    isHave = 1;
	    break;
	}
    return isHave;
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
