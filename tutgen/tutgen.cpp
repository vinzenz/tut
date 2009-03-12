#include "tutgen.h"
#include "parser.h"
#include "config.h"

Parser *parser;
Config *config;

int main(int argc, char *argv[])
{
	config = new Config();	
	parser = new Parser( (argc > 1 && strcmp(argv[1],"-v") == 0) ? true : false );

	std::vector< std::string > directories = config->GetValue( "tutgen.cfg", "dir" );
	
	//for( unsigned int i = 0; i < directories.size(); i++ ) {
	//	parser->Parse( directories[i] );
	//}

	parser->Parse( "." );

	std::map<std::string,std::string> values = parser->PrepareVariables();
	

	config->PrepareDirectory( directories[0] );
	config->GenerateFromConfig( "tutgen.cfg", directories[0]+"/main.cpp", "main", values );
	config->GenerateFromConfig( "tutgen.cfg", directories[0]+"/tests.cpp", "tests", values );
	config->GenerateFromConfig( "tutgen.cfg", directories[0]+"/Makefile", "make", values );

	delete parser;
	delete config;
	return 0;
}
