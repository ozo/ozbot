#include "parsemodul.h"

#include <map>
#include <vector>

#include <gloox/client.h>
#include <gloox/mucroom.h>

#include "../analyzemsg.h"

int ParseModul::Writer( char *data, size_t size, size_t nmemb, std::string *buffer ){
    int result = 0;
    buffer->append( data, size * nmemb );
    result = size * nmemb;
    return result;
}

ParseModul::ParseModul( gloox::Client *cl )
    : Modul( cl )
    , citys( new std::map< std::string, std::string > ){
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_HEADER, 0);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Writer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    
    citys->insert( std::make_pair( "ЖЛОБИН",    "26966" ) );
    citys->insert( std::make_pair( "ГОМЕЛЬ",    "33041" ) );
    citys->insert( std::make_pair( "БРЕСТ",     "33008" ) );
    citys->insert( std::make_pair( "ВИТЕБСК",   "26666" ) );
    citys->insert( std::make_pair( "ГРОДНО",    "26825" ) );
    citys->insert( std::make_pair( "МИНСК",     "26850" ) );
    citys->insert( std::make_pair( "МОГИЛЁВ",   "26863" ) );
    citys->insert( std::make_pair( "СОЛИГОРСК", "26003" ) );
    citys->insert( std::make_pair( "СЛУЦК",     "26951" ) );
    
    AddMode( "!BOR", "Получить 5 последних цитат с bash.org.ru, bash.org.by, ibash.org.ru : !bor" );
    AddMode( "!WEATHER", "Получить погоду на завтра для города Белоруссии : !weather город" );
    AddMode( "!IBASH", "Получить случайную цитату с ibash.org.ru : !ibash" );
    AddMode( "!LORQ",  "Получить последнию цитату с lorquotes.ru : !lorq"  );
    AddMode( "!CALENDAR" );
}

ParseModul::~ParseModul(){
    curl_easy_cleanup(curl);
    delete citys;
}

bool ParseModul::Message( gloox::MUCRoom* room
				  , const std::string &normal
				  , const std::string &upper
				  , const gloox::JID &from
			          ,  bool priv ){
    std::string result;
    ParseMode mode = BOR;
    const std::string FIRST_WORD( getWord( upper, 0 )  );
    
    const int MODES_LENGTH = 5;
    const std::string modes[ MODES_LENGTH ] = {   "!BOR"
						, "!WEATHER"
						, "!IBASH"
						, "!LORQ"
						, "!CALENDAR" };

    std::string city = citys->find( "ЖЛОБИН" )->second;
    bool isHave = 0;

    for( int i = 0; i < MODES_LENGTH; ++i )
	if( modes[ i ] == FIRST_WORD ){
	    switch( i ){
	    case 0 : mode = BOR;
		     result = Bor( 5 );
		     break;
	    case 1 : mode = WEATHER;
		     if( numberOfWords( upper ) > 1 )
			 if( citys->find( getWord( upper, 1 ) ) != citys->end() )
			     city = citys->find( getWord( upper, 1 ) )->second;
		     result = Weather( city );
		     break;
	    case 2 : mode = IBASH;
		     result = GetDataFromUrl("http://www.infostar.ua/forum/img/bash.php");
		     break;
	    case 3 : mode = LORQ;
	 	     result = GetDataFromUrl("http://www.infostar.ua/forum/img/lorq.php");
		     break;
	    case 4 : mode = CALENDAR;
		     result = GetDataFromUrl("http://www.infostar.ua/forum/img/calend.php");
		     break;
	    }
	    isHave = 1;
	}

    if( !isHave )
	return 0;

    std::string modeName;
    switch( mode ){
    case BOR      : modeName = "bor";      break;
    case WEATHER  : modeName = "weather";  break;
    case IBASH    : modeName = "ibash";    break;
    case LORQ     : modeName = "lorq";     break;
    case CALENDAR : modeName = "calendar"; break;
    }

    result = ( result.empty() ? "Ошибка, насяльника !" : result );
    if( priv )
	Send( from, result, modeName );
    else
	room->send( result );
    
    return 1;

}

std::string ParseModul::Bor( unsigned int howMany = 0 ){
    CURLcode curlResult;
    howMany = ( howMany > 5 || howMany <= 0 ) ? 1 : howMany;

    curl_easy_setopt( curl, CURLOPT_URL, "http://bashh.blog.tut.by/" );
    curlResult = curl_easy_perform(curl);

    if( curlResult == CURLE_OK ){
	std::vector< std::string > quotes;
	std::string tmp;
	const std::string QUOTE_BEGIN( "<div class=\"entry-body\">" );
	const std::string QUOTE_END(   "<div" );
 
	while( buffer.find( QUOTE_BEGIN ) != std::string::npos && quotes.size() < howMany ){
	    EraseWith( buffer, QUOTE_BEGIN );
	    tmp = buffer.substr( 0, buffer.find( QUOTE_END ) );

	    while( !tmp.empty() )
		if( (int)tmp[ 0 ] == 9 || (int)tmp[ 0 ] == 10 )
		    tmp.erase( 0, 1 );
		else 
		    break;
	    
	    Replace( tmp, "<br />",  ""   );
	    Replace( tmp, "<br>",    "\n" );
	    Replace( tmp, "&#60;",   "<"  );
	    Replace( tmp, "&#62;",   ">"  );
	    Replace( tmp, "&lt;",    "<"  );
	    Replace( tmp, "&gt;",    ">"  );
	    Replace( tmp, "&quot;", "\"" );
	    Replace( tmp, "&#039;", "'" );

	    quotes.push_back( tmp );
	}
	buffer.erase();
	std::string result;
	for( std::vector< std::string >::const_iterator i = quotes.begin(); i != quotes.end(); ++i )
	    result += *i + "\n==============\n";
	return result;
    } else {
	buffer.erase();
	return "";
    }
}

std::string ParseModul::Weather( const std::string &city ){
    CURLcode curlResult;
    const std::string url = "http://pogoda.tut.by/city/" + city + ".html?detail=1";
    curl_easy_setopt( curl, CURLOPT_URL, url.c_str() );
    curlResult = curl_easy_perform(curl);

    if( curlResult == CURLE_OK ){

	const std::string NEW_DAY_BEGIN( "<div class=\"calendar_den" );
	const std::string NOON_BEGIN( "<td class=\"t_l\"><div class=\"" );

	for( int i = 0; i < 2; ++i )
	    EraseWith( buffer, NEW_DAY_BEGIN );
	for( int i = 0; i < 2; ++i )
	    EraseWith( buffer, NOON_BEGIN );

	std::string weather;
	if( buffer.find( '\"' ) != std::string::npos )
	    weather = buffer.substr( 0, buffer.find( '\"') );
	EraseWith( buffer, ">" );
	std::string temperature;
	if( buffer.find( '&' ) != std::string::npos )
	    temperature = buffer.substr( 0, buffer.find( '&' ) );
	buffer.erase();

	if( weather.empty() || temperature.empty() )
	    return "Ошибка получения данных";

	const int MAX_ID = 6;
	const std::string id[] = {   "w_d_7"
			           , "w_d_10"
			           , "w_d_6"
			           , "w_d_11"
			           , "w_d_8"
				   , "w_d_9"
	};
	const std::string weathers[] = { "ясно"
				       , "переменная облачность, небольшой дождь"
				       , "переменная облачность"
				       , "облачно, небольшой дождь"
				       , "дождь, гроза"
				       , "переменная облачность, дождь"
	};

	for( int i = 0; i < MAX_ID; ++i )
	    if( weather == id[ i ] ){
		weather = weathers[ i ];
		break;
	    }

	return ( "Завтра там будет " + weather + ", при температуре " + temperature + "°C");
    } else {
	buffer.erase();
	return "";
    }
}

std::string ParseModul::GetDataFromUrl( const std::string &url ) const {
    CURLcode curlResult;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
    curlResult = curl_easy_perform(curl);

    if( curlResult == CURLE_OK ){
	std::string result = buffer;
	buffer.erase();
        return result;
    } else {
        buffer.erase();
        return "";
    }
}
