#include "config.h"
#include "parser.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>


Config::Config()
{
}

Config::~Config()
{
}

// TODO: sprawdzanie poprawnosci plikow!!!
bool Config::GenerateFromConfig( std::string fileIn, std::string fileOut, std::string tag, std::map< std::string, std::string> values )
{
	std::ifstream fileStream;
    fileStream.open( fileIn.c_str() );
    std::string fileContent((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
    fileStream.close();

	MatchResult matchResult = Parser::FindMatch( fileContent, "\\["+tag+"\\](.*)\\[/"+tag+"\\]" );
	std::string replaceResult = matchResult.SubMatches()[0];
	for( std::map< std::string, std::string>::iterator i = values.begin(); i != values.end(); i++ )
	{
		std::cout << (*i).first;	
		if( matchResult.SubMatches()[0].find( "@"+(*i).first+"@" ) != std::string::npos )
			replaceResult = replaceResult.replace( replaceResult.find( "@"+(*i).first+"@" ), ( "@"+(*i).first+"@" ).length(), (*i).second );
	}

	std::ofstream fileOutStream;
	fileOutStream.open( fileOut.c_str() );
	fileOutStream << replaceResult;
	fileOutStream.close();

	return true;
}

std::vector< std::string > Config::GetValue( std::string file, std::string value )
{
	std::ifstream fileStream;
    fileStream.open( file.c_str() );
    std::string fileContent((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
    fileStream.close();

	std::vector< std::string > values;
	MatchResult matchResult = MatchResult( false, fileContent, std::vector< std::string> () );
	while( (matchResult = Parser::FindMatch( matchResult.TextUpdated(), value+":([^\n]*)" )).WasMatch() )
	{
		values.push_back( matchResult.SubMatches()[0] );
	}

	return values;
}

void Config::PrepareDirectory( std::string directory )
{
	mkdir( directory.c_str(),0777 ); 
}
