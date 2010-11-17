#include "parsemodul.h"

#include <map>
#include <vector>
#include <boost/lexical_cast.hpp>

#include <gloox/client.h>
#include <gloox/mucroom.h>

#include "../analyzemsg.h"
#include "../loger.h"

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
    commands.AddCommand( "!BOR", &ParseModul::Bor );
    values.insert( std::make_pair( "!BOR", "5") );

    AddMode( "!WEATHER", "Получить погоду на завтра для города Белоруссии : !weather город" );
    commands.AddCommand( "!WEATHER", &ParseModul::Weather );
    values.insert( std::make_pair( "!WEATHER", "" ) );

    AddMode( "!IBASH", "Получить случайную цитату с ibash.org.ru : !ibash" );
    commands.AddCommand( "!IBASH", &ParseModul::GetDataFromUrl );
    values.insert( std::make_pair( "!IBASH", "http://www.infostar.ua/forum/img/bash.php" ) );

    AddMode( "!LORQ",  "Получить последнию цитату с lorquotes.ru : !lorq"  );
    commands.AddCommand( "!LORQ", &ParseModul::GetDataFromUrl );
    values.insert( std::make_pair( "!LORQ", "http://www.infostar.ua/forum/img/lorq.php" ) );

    AddMode( "!WIKI", "Поиск по вики linuxportal.by : !wiki соловосочетание" );
    commands.AddCommand( "!WIKI", &ParseModul::Wiki );
    values.insert( std::make_pair( "!WIKI", "" ) );

    AddMode( "!CALENDAR" );
    commands.AddCommand( "!CALENDAR", &ParseModul::GetDataFromUrl );
    values.insert( std::make_pair( "!CALENDAR"
				   , "http://www.infostar.ua/forum/img/calend.php" ) );
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
    const std::string COMMAND( getWord( upper, 0 )  );
    std::string COMMAND_LINE;
    if( values.find( COMMAND ) != values.end( ) )
	COMMAND_LINE = values.find( COMMAND )->second;
    else
	return 0;
    if( COMMAND_LINE.empty( ) )
	COMMAND_LINE = ( numberOfWords( normal ) > 1 ? getWordsFrom( normal, 1 ) : "" );

    if( commands.GetPtr( COMMAND ) ){
	std::string result = ( ( this->*( commands.GetPtr( COMMAND ) ) )( room, COMMAND_LINE ) );
	if( result.empty( ) )
	    result = "Ошибка, насяльника !";
	Send( room, from, result, priv );
	return 1;
    }
    return 0;
}

std::string ParseModul::Bor( gloox::MUCRoom*, const std::string &number ){
    unsigned int howMany;
    try{
	howMany = boost::lexical_cast< unsigned int >( number );
    } catch( boost::bad_lexical_cast ){
	return "";
    }
    const std::string url = "http://bashh.blog.tut.by/";
    std::string source = GetDataFromUrl( 0, url );
    if(  source.empty( ) )
	return "";

    std::vector< std::string > quotes;
    std::string tmp;
    const std::string QUOTE_BEGIN( "<div class=\"entry-body\">" );
    const std::string QUOTE_END(   "<div" );
    
    while( source.find( QUOTE_BEGIN ) != std::string::npos && quotes.size() < howMany ){
	EraseWith( source, QUOTE_BEGIN );
	tmp = source.substr( 0, source.find( QUOTE_END ) );
	
	while( !tmp.empty() )
	    if( (int)tmp[ 0 ] == 9 || (int)tmp[ 0 ] == 10 )
		tmp.erase( 0, 1 );
	    else 
		break;

	Replace( tmp, "<p>", "" );
	Replace( tmp, "</p>", "\n" );
	Replace( tmp, "&#8211;", "-" );
	Replace( tmp, "&#8230;", "..." );
	Replace( tmp, "<br />",  "\n"   );
	Replace( tmp, "<br>",    "\n" );
	Replace( tmp, "&#60;",   "<"  );
	Replace( tmp, "&#62;",   ">"  );
	Replace( tmp, "&lt;",    "<"  );
	Replace( tmp, "&gt;",    ">"  );
	Replace( tmp, "&quot;", "\"" );
	Replace( tmp, "&#039;", "'" );
	
	quotes.push_back( tmp );
    }
    source.erase( );
    std::string result;
    for( std::vector< std::string >::const_iterator i = quotes.begin(); i != quotes.end(); ++i )
	result += *i + "\n==============\n";
    return result;
}

std::string ParseModul::Weather( gloox::MUCRoom*, const std::string &cmd ){
    std::string city;
    if( numberOfWords( cmd ) ){
	if( citys->find( getWord( toUpper( cmd ), 0 ) ) != citys->end() )
	    city = citys->find( getWord( toUpper( cmd ), 0 ) )->second;
	else 
	    city = citys->find( "ГОМЕЛЬ" )->second;
    } else
	city = citys->find( "ГОМЕЛЬ" )->second;
    const std::string url = "http://pogoda.tut.by/city/" + city + ".html?detail=1";
    std::string source = GetDataFromUrl( 0, url );
    if( source.empty( ) )
	return "";

    const std::string NEW_DAY_BEGIN( "<div class=\"calendar_den" );
    const std::string NOON_BEGIN( "<td class=\"t_l\"><div class=\"" );
    
    for( int i = 0; i < 2; ++i )
	EraseWith( source, NEW_DAY_BEGIN );
    for( int i = 0; i < 2; ++i )
	EraseWith( source, NOON_BEGIN );
    
    std::string weather;
    if( source.find( '\"' ) != std::string::npos )
	weather = source.substr( 0, source.find( '\"') );
    EraseWith( source, ">" );
    std::string temperature;
    if( source.find( '&' ) != std::string::npos )
	temperature = source.substr( 0, source.find( '&' ) );
    source.erase();
    
    if( weather.empty() || temperature.empty() )
	return "";
    
    const int MAX_ID = 7;
    const std::string id[ MAX_ID ] = {   "w_d_7"
				 , "w_d_10"
				 , "w_d_6"
				 , "w_d_11"
				 , "w_d_8"
				 , "w_d_9"
				 , "w_d_1"
    };
    const std::string weathers[ MAX_ID ] = { "ясно"
				     , "переменная облачность, небольшой дождь"
				     , "переменная облачность"
				     , "облачно, небольшой дождь"
				     , "дождь, гроза"
				     , "переменная облачность, дождь"
				     , "дождь"
    };
    
    for( int i = 0; i < MAX_ID; ++i )
	if( weather == id[ i ] ){
	    weather = weathers[ i ];
	    break;
	}
    
    return ( "Завтра там будет " + weather + ", при температуре " + temperature + "°C");
}

std::string ParseModul::Wiki( gloox::MUCRoom*, const std::string &quaryString ){
    if( quaryString.empty( ) )
	return "";

    std::string quary = quaryString;
    Replace( quary, " ", "+" );
    const std::string server = "http://wiki.linuxportal.by";
    std::string url = server + "/index.php?ns0=2&search=" 
	+ quary 
	+ "&searchx=Искать";
    std::string result = GetDataFromUrl( 0, url );
    if( result.empty( ) )
	return "";

    const std::string TITLE_BEGIN = "<h2>";
    const std::string TITLE_END   = "</h2>";
    if( result.find( TITLE_BEGIN ) != std::string::npos )
	EraseWith( result, TITLE_BEGIN );
    else
	return "";
    std::string title = result.substr( 0, result.find( TITLE_END ) );
    EraseWith( result, TITLE_END );
    if( result.find( TITLE_BEGIN ) != std::string::npos )
	result.erase( result.find( TITLE_BEGIN )  );
 
    const std::string PARAGRAPH_BEGIN = "<p>";
    if( result.find( PARAGRAPH_BEGIN ) != std::string::npos )
	result.erase( result.find( PARAGRAPH_BEGIN )  );

    std::list< std::pair< std::string, std::string > > urls;
    std::string pageName;
    const std::string URL_BEGIN       = "<a href=\"";
    const std::string PAGE_NAME_BEGIN = "title=\"";
    const std::string END             = "\"";
    while( result.find( URL_BEGIN ) != std::string::npos  ){
	EraseWith( result, URL_BEGIN );
	url = result.substr( 0, result.find( END ) );
	if( result.find( PAGE_NAME_BEGIN ) != std::string::npos ){
	    EraseWith( result, PAGE_NAME_BEGIN );
	    pageName = result.substr( 0, result.find( END ) );
	}
	urls.push_back( std::make_pair( server + url, pageName ) );
    }
    result.erase( );
    result += title;
    for( std::list< std::pair< std::string, std::string > >::iterator i = urls.begin( )
	     ; i != urls.end(  ); ++i )
	result += "\nНазвание страницы: " + i->second + "\nURL : " + i->first;
    return result;
}

std::string ParseModul::GetDataFromUrl( gloox::MUCRoom*, const std::string &url ){
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
