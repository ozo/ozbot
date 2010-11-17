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
	     "VISITOR - посетитель\n"
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
    const std::string commandName  = getWord( upper, 0 );
    if( !commands.GetPtr( commandName ) )
	return 0;

    if( !IsRoot( from.full() ) ){
	NotPermissed( from, room, priv );
	return 0;
    }

    if( numberOfWords( normal ) < 2 ){
	LowLength( room, from, priv );
	return 0;
    }

    const std::string cmd          = getWordsFrom( normal, 1 );
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
    const std::string user = getWordsFrom( cmd, 1 );

    std::map< std::string, gloox::MUCRoomRole > roles;
    roles.insert( std::make_pair( "NONE", gloox::RoleNone ) );
    roles.insert( std::make_pair( "GUEST", gloox::RoleVisitor ) );
    roles.insert( std::make_pair( "VISITOR", gloox::RoleParticipant ) );
    roles.insert( std::make_pair( "MODERATOR", gloox::RoleModerator ) );
    if( roles.find( role ) != roles.end( ) )
	room->setRole( user, roles.find( role )->second );
    else
	return 0;
    return 1;
}

bool AdminModul::SetAff( gloox::MUCRoom *room, const std::string &cmd ){
    if( numberOfWords( cmd ) < 2 )
	return 0;

    const std::string aff  = getWord( toUpper( cmd ), 0 );
    const std::string user = getWordsFrom( cmd, 1 );

    std::map< std::string, gloox::MUCRoomAffiliation > affs;
    affs.insert( std::make_pair( "NONE", gloox::AffiliationNone ) );
    affs.insert( std::make_pair( "OUTCAST", gloox::AffiliationOutcast ) );
    affs.insert( std::make_pair( "MEMBER", gloox::AffiliationMember ) );
    affs.insert( std::make_pair( "OWNER", gloox::AffiliationOwner ) );
    affs.insert( std::make_pair( "ADMIN", gloox::AffiliationAdmin ) );
    if( affs.find( aff ) != affs.end( ) )
	room->setAffiliation( user, affs.find( aff )->second, "Так было велено мне" );
    else
	return 0;
    return 1;
}

gloox::Presence::PresenceType AdminModul::GetPresence( const std::string &from ) const {
    std::map< std::string, gloox::Presence::PresenceType > presentes;
    presentes.insert( std::make_pair( "ONLINE", gloox::Presence::Available ) );
    presentes.insert( std::make_pair( "CHAT", gloox::Presence::Chat ) );
    presentes.insert( std::make_pair( "AWAY", gloox::Presence::Away ) );
    presentes.insert( std::make_pair( "DND", gloox::Presence::DND ) );
    presentes.insert( std::make_pair( "XA", gloox::Presence::XA ) );
    if( presentes.find( from ) != presentes.end( ) )
	return presentes.find( from )->second;
    else
	return gloox::Presence::Error;
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
