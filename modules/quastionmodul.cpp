#include "quastionmodul.h"

#include "../analyzemsg.h"
#include "../quastion.h"

#include <fstream>
#include <algorithm>
#include <sstream>

#include <gloox/mucroom.h>
#include <gloox/messagesession.h>
#include <gloox/client.h>

QuastionModul::QuastionModul( gloox::Client *cl, const std::string &quastionFile )
    : FileModul( cl, quastionFile ){
 
    qFile = quastionFile;

    std::stringstream stream;
    for( FileModul::StringList::const_iterator i = content.begin()
	     ; i != content.end(); ++i )
	stream << *i << '\n';

    while( !stream.eof() ){
	Quastion tmp;
	stream >> tmp;
	if( !tmp.Empty() )
	    qList.push_back( tmp );
    }

    AddMode("!Q", "Задать вопрос пользователя конференции : !q nick вопрос\n"
	    "Вопрос будет доставлен при входе пользователя с данным ником в концеренцию\n"
	    "От одного участника одновременно возможен лишь один вопрос\n"
	    "Ответить на вопрос можно с помощью команды !a");
    AddMode("!A", "Ответить на вопрос пользователя : !a nick ответ");

}

bool QuastionModul::Message( gloox::MUCRoom* room
			, const std::string &normal
			, const std::string &upper
			, const gloox::JID &fromJid
			,  bool priv ){
    const std::string FIRST_WORD = getWord( upper, 0 );
    const int         MSG_LENGTH = numberOfWords( normal );

    if( FIRST_WORD != "!A" && FIRST_WORD != "!Q" )
	return 0;

    if( MSG_LENGTH < 3 ){
	LowLength( room, fromJid, priv );
	return 0;
    }

    const std::string to   = room->name() + "@" + room->service() + "/" + getWord( normal, 1 );
    const std::string msg  = getWordsFrom( normal, 2 );
    const std::string from = fromJid.full();
    
    if( FIRST_WORD == "!Q" ){
	if( IsHaveUser( to ) ){
	    std::string msg = "он и так в конференции";
	    Send( room, fromJid, msg, priv );
	    return 1;
	}
	for( std::list< Quastion >::iterator i = qList.begin(); i != qList.end(); )
	    if( i->GetFrom() == from ){
		qList.erase( i );
		i = qList.begin();
	    } else
		++i;
	qList.push_back( Quastion( msg, from, to ) );
	WriteQuastions();
    } else 
	if( FIRST_WORD == "!A" ){
	    for( std::list< Quastion >::iterator i = qList.begin(); i != qList.end(); ++i )
		if( i->GetFrom() == to ){
		    if( i->GetTo() == from ){
			if( IsHaveUser( to ) ){
			    std::string msg = "пользователь " + i->GetTo() 
				+ " передал вам ответ : \" " + i->GetAnswer() + '\"';
			    Send( to, msg, "answer" );
			    qList.erase( i );
			    break;
			} else {
			    i->SetAnswer( msg );
			}
			WriteQuastions();
		    } else {
			Send( from, "вопрос не вам", "answer" );
		    }
		}
	}
    return 1;
}
    
void QuastionModul::AddUser( const std::string &userFull ){

    if( !IsHaveUser( userFull ) )
	users.push_back( userFull );
    else
	return;

    for( std::list< Quastion >::iterator i = qList.begin(); i != qList.end(); ++i )
	if( i->GetTo() == userFull && i->GetAnswer() == "" ){
	    std::string msg = "Пользователь " + i->GetFrom() + " передал вам сообщение : \"" + i->GetQuastion() + '\"';
	    Send( userFull, msg, "quastion" );
	}
    for( std::list< Quastion >::iterator i = qList.begin(); i != qList.end(); )
	if( i->GetFrom() == userFull && i->GetAnswer() != "" ){
	    std::string msg = "Пользователь " + i->GetTo() + " передал вам ответ : \"" + i->GetAnswer() + '\"';
	    Send( userFull, msg, "answer" );
	    qList.erase( i );
	    i = qList.begin();
	    WriteQuastions();
	} else
	    ++i;

}

void QuastionModul::DeleteUser( const std::string &user ){
    if( IsHaveUser( user ) )
	users.erase( std::find( users.begin(), users.end(), user ) );
}

bool QuastionModul::IsHaveUser( const std::string &user ){
    return ( std::find( users.begin(), users.end(), user ) != users.end() );
}

void QuastionModul::WriteQuastions(){
    std::ofstream out( qFile.c_str() );
    if( out ){
	for( std::list<Quastion>::const_iterator i = qList.begin(); i != qList.end(); ++i )
	    out << *i;
	out.close();
    }
}
