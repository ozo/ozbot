#ifndef QUASTION_H
#define QUASTION_H

#include <string>

#include <istream>
#include <ostream>

class Quastion{
 public:
    friend std::ostream &operator << (std::ostream &, const Quastion & );
    friend std::istream &operator >> (std::istream &, Quastion & );
    Quastion(){}
    Quastion(const Quastion &from );
    Quastion(const std::string &quastion, const std::string &from, const std::string &to );
    void SetQuastion(const std::string &quastion
		     , const std::string &from
		     , const std::string &to);
    void SetAnswer(const std::string &answer){
	this->answer = answer;
    }
    std::string GetAnswer() const {
	return answer;
    }
    std::string GetFrom() const {
	return from;
    }
    std::string GetTo() const {
	return to;
    }
    std::string GetQuastion() const {
	return quastion;
    }
    bool Empty(){
	if( from.empty() || quastion.empty() || to.empty() )
	    return 1;
	else
	    return 0;
    }

 protected:
    std::string from;
    std::string to;
    std::string quastion;
    std::string answer;
};

#endif
