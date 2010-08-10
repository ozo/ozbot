#include "quastion.h"

Quastion::Quastion(const Quastion &from )
    : from( from.from )
    , to( from.to )
    , quastion( from.quastion )
    , answer( from.answer ){

}

Quastion::Quastion(const std::string &q, const std::string &f, const std::string &t )
    : from( f )
    , to( t )
    , quastion( q )
    , answer( "" ){
}

std::ostream &operator << ( std::ostream &output, const Quastion &quastion ){
    output << quastion.quastion << '\n';
    output << quastion.from << '\n';
    output << quastion.to << '\n';
    output << quastion.answer << '\n';
    return output;
}

std::istream &operator >> ( std::istream &input, Quastion &quastion ){
    std::string tmp[4];//Quastion, tmpFrom, tmpTo, tmpAnswer;
    for( int i = 0; i < 4; ++i )
	std::getline( input, tmp[i] );
    quastion.SetQuastion( tmp[0]/*Quastion*/, tmp[1]/*From*/, tmp[2]/*tmpTo*/);
    quastion.SetAnswer( tmp[3]/*Answer*/);
    return input;
}

void Quastion::SetQuastion(const std::string &quastion, const std::string &from, const std::string &to){
    this->quastion = quastion;
    this->from = from;
    this->to = to;
}
