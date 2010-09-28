#include "quastionmodul.h"

#include "../analyzemsg.h"

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

    if( !stream.eof( ) ){
	Quastion tmp;
	while( ReadQuastion( stream, tmp ) )
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
	for( std::list< Quastion >::iterator i = qList.begin(); i != qList.end(); ++i )
	    if( i->from == from ){
		i = qList.erase( i );
		--i;
	    }
	Quastion tmp = { from, to, msg, "" };
	qList.push_back( tmp );
	WriteQuastions();
    } else 
	if( FIRST_WORD == "!A" ){
	    for( std::list< Quastion >::iterator i = qList.begin(); i != qList.end(); ++i )
		if( i->from == to ){
		    if( i->to == from ){
			if( IsHaveUser( to ) ){
			    std::string msg = "пользователь " + i->to 
				+ " передал вам ответ : \" " + i->answer + '\"';
			    Send( to, msg, "answer" );
			    qList.erase( i );
			    break;
			} else {
			    i->answer = msg;
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
	if( i->to == userFull && i->answer.empty( ) ){
	    std::string msg = "Пользователь " + i->from + " передал вам сообщение : \"" 
		+ i->quastion + '\"';
	    Send( userFull, msg, "quastion" );
	}
    for( std::list< Quastion >::iterator i = qList.begin(); i != qList.end(); ++i )
	if( i->from == userFull && !i->answer.empty( ) ){
	    std::string msg = "Пользователь " + i->to + " передал вам ответ : \"" 
		+ i->answer + '\"';
	    Send( userFull, msg, "answer" );
	    i = qList.erase( i );
	    WriteQuastions();
	    --i;
	}
}

void QuastionModul::DeleteUser( const std::string &user ){
    if( IsHaveUser( user ) )
	users.erase( std::find( users.begin(), users.end(), user ) );
}

bool QuastionModul::IsHaveUser( const std::string &user ){
    return ( std::find( users.begin(), users.end(), user ) != users.end() );
}

bool QuastionModul::ReadQuastion( std::istream &stream, Quastion &quastion ){
    bool isGoodRead = ( std::getline( stream, quastion.quastion)
			&& std::getline( stream, quastion.from)
			&& std::getline( stream, quastion.to ) );
    std::getline( stream, quastion.answer );
    return isGoodRead;
}
 
void QuastionModul::WriteQuastion( std::ostream &stream, const Quastion &quastion ){
    stream << quastion.quastion << '\n';
    stream << quastion.from << '\n';
    stream << quastion.to << '\n';
    stream << quastion.answer << '\n';
}

void QuastionModul::WriteQuastions(){
    std::ofstream out( qFile.c_str() );
    if( out ){
	for( std::list<Quastion>::const_iterator i = qList.begin(); i != qList.end(); ++i )
	    WriteQuastion( out, *i );
	out.close();
    }
}
