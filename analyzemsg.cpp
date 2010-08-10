#include "analyzemsg.h"

#include <algorithm>
#include <cstring>
#include <string>

int numberOfWords( const std::string &str){
    if( str.empty() )
	return 0;

    int number = 0;
    for( unsigned int i = 0; i < str.length() - 1; ++i )
	if( str[ i ] != ' ' && str[ i + 1 ] == ' ' )
	    ++number;
    if( str[ str.length() - 1 ] != ' ' )
	++number;
    return number;
}

std::string getWord( const std::string &str, const int from ){
    if( numberOfWords( str ) <= from )
	return "";

    std::string tmp;
    if( str.find_first_not_of( ' ' ) != std::string::npos )
	tmp = str.substr( str.find_first_not_of( ' ' ) );
    
    for( int i = 0; i < from; ++i ){
	tmp.erase( 0, tmp.find( ' ' ) );
	tmp.erase( 0, tmp.find_first_not_of( ' ' ) );
    }

    if( tmp.find( ' ' ) == std::string::npos )
	return tmp;
    else
	return tmp.substr( 0, tmp.find( ' ' ) );
}

std::string getWordsFrom( const std::string &str, const int from ){
    if( numberOfWords( str ) <= from )
	return "";

    std::string tmp = str.substr( str.find_first_not_of( ' ' ) );
    for( int i = 0; i < from; ++i ){
	tmp.erase( 0, tmp.find( ' ' ) );
	tmp.erase( 0, tmp.find_first_not_of( ' ' ) );
    }
    while( !tmp.empty() && tmp[ tmp.length() - 1 ] == ' ' )
	tmp.erase( tmp.length() - 1, 1 );

    return tmp;
};

std::string narrow(const std::wstring& wide, const std::locale& locale = std::locale("") )
{
    if( wide.empty() )
        return std::string();

    typedef std::wstring::traits_type::state_type state_type;
    typedef std::codecvt<wchar_t, char, state_type> CVT;

    const CVT& cvt = std::use_facet<CVT>(locale);
    std::string narrow(cvt.max_length()*wide.size(), '\0');
    state_type state = state_type();

    const wchar_t* from_beg = &wide[0];
    const wchar_t* from_end = from_beg + wide.size();
    const wchar_t* from_nxt;
    char* to_beg = &narrow[0];
    char* to_end = to_beg + narrow.size();
    char* to_nxt;

    std::string::size_type sz = 0;
    std::codecvt_base::result r;
    do
    {
        r = cvt.out(state, from_beg, from_end, from_nxt,
                           to_beg,   to_end,   to_nxt);
        switch (r)
        {
        case std::codecvt_base::error:
	    return narrow;

        case std::codecvt_base::partial:
            sz += to_nxt - to_beg;
            narrow.resize(2*narrow.size());
            to_beg = &narrow[sz];
            to_end = &narrow[0] + narrow.size();
            break;

        case std::codecvt_base::noconv:
            narrow.resize(sz + (from_end-from_beg)*sizeof(wchar_t));
            std::memcpy(&narrow[sz], from_beg,(from_end-from_beg)*sizeof(wchar_t));
            r = std::codecvt_base::ok;
            break;

        case std::codecvt_base::ok:
            sz += to_nxt - to_beg;
            narrow.resize(sz);
            break;
        }
    } while( r != std::codecvt_base::ok );

    return narrow;
} 

std::wstring widen(const std::string& narrow, const std::locale& locale = std::locale("") )
{
    if( narrow.empty() )
        return std::wstring();

    typedef std::string::traits_type::state_type state_type;
    typedef std::codecvt<wchar_t, char, state_type> CVT;

    const CVT& cvt = std::use_facet<CVT>(locale);
    std::wstring wide(narrow.size(), '\0');
    state_type state = state_type();
    const char* from_beg = &narrow[0];
    const char* from_end = from_beg + narrow.size();
    const char* from_nxt;
    wchar_t* to_beg = &wide[0];
    wchar_t* to_end = to_beg + wide.size();
    wchar_t* to_nxt;

    std::wstring::size_type sz = 0;
    std::codecvt_base::result r;
    do
    {
        r = cvt.in(state, from_beg, from_end, from_nxt,
                          to_beg,   to_end,   to_nxt);
        switch (r)
        {
        case std::codecvt_base::error:
	    return wide;

        case std::codecvt_base::partial:
            sz += to_nxt - to_beg;
            wide.resize(2*wide.size());
            to_beg = &wide[sz];
            to_end = &wide[0] + wide.size();
            break;

        case std::codecvt_base::noconv:
            wide.resize(sz + (from_end-from_beg));
            std::memcpy(&wide[sz], from_beg, (std::size_t)(from_end-from_beg));
            r = std::codecvt_base::ok;
            break;

        case std::codecvt_base::ok:
            sz += to_nxt - to_beg;
            wide.resize(sz);
            break;
        }
    } while( r != std::codecvt_base::ok );

   return wide;
}

std::string toUpper( const std::string &str ){
    std::wstring tmp = widen( str );
    std::for_each( tmp.begin(), tmp.end(), toWupper );
    return narrow( tmp );
}
