#ifndef MESSAGE_LOGER_H
#define MESSAGE_LOGER_H

#include <list>
#include <string>

class MessageLoger{
public:
    void AddMessage(const std::string &resource
		    , const std::string &user
		    , const std::string &message );
    std::string GetMessages( const int length, const std::string &resource ) const;
    std::string GetFull(const std::string &resource ) const;
protected:
    struct Message{
	std::string resource;
	std::string user;
	std::string message;
    };

    typedef std::list< Message > MessageList;

    MessageList log;
};
#endif
