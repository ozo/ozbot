#ifndef ANALYZEMSG_H
#define ANALYZEMSG_H

#include <list>
#include <string>
#include <locale>

int numberOfWords( const std::string &str);
std::string getWord( const std::string &str, const int from );
std::string getWordsFrom( const std::string &str, const int from );

std::string narrow(const std::wstring& wide, const std::locale& locale );
std::wstring widen(const std::string& narrow, const std::locale& locale );

inline void toWupper( wchar_t &ch ){
    ch = std::use_facet< std::ctype< wchar_t > >( std::locale("") ).toupper( ch );
}

std::string toUpper( const std::string &str );

#endif
