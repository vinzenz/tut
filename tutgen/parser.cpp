#include  "parser.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

inline std::string trim_right(const std::string &source , const std::string& t = " ")
{
	std::string str = source;
	return str.erase( str.find_last_not_of(t) + 1);
}

inline std::string trim_left( const std::string& source, const std::string& t = " ")
{
	std::string str = source;
	return str.erase(0 , source.find_first_not_of(t) );
}

inline std::string trim(const std::string& source, const std::string& t = " ")
{
	std::string str = source;
	return trim_left( trim_right( str , t) , t );
}


Parser::Parser( bool verbose)
: verbose(verbose), basedir("")
{
	
}

Parser::~Parser()
{
	for( std::vector< Entry* >::iterator i = entries.begin(); i < entries.end(); i++ )
		delete *i;
}

void Parser::Parse( std::string directory )
{ 
	basedir = directory;
	
	DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(directory.c_str())) == NULL) {
        std::cout << "Error(" << errno << ") opening " << directory << std::endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
		if( std::string(dirp->d_name).find_last_of(".") != std::string::npos &&
			( std::string(dirp->d_name).substr(std::string(dirp->d_name).find_last_of(".")) == ".cpp"
		   || std::string(dirp->d_name).substr(std::string(dirp->d_name).find_last_of(".")) == ".h") )
		{
        	ParseFile( dirp->d_name );
		}
    }
    closedir(dp);


	//for( std::vector< std::string >::iterator i = files.begin(); i < files.end(); i++ )
		
}

void Parser::ParseFile( std::string file )
{
	for( unsigned int i = 0; i < files.size(); i++ )
	{
		if( files[i] == basedir+"/"+file ||
			file == "tests.cpp" || file == "main.cpp" )
			return;
	}	
	files.push_back( basedir+"/"+file );

	if( verbose )		
		std::cout << "Start processing file:\t" << (basedir+"/"+file) << std::endl;	

	std::ifstream fileStream;
	fileStream.open( (basedir+"/"+file).c_str() );
	std::string fileContent((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
	fileStream.close();
    
	ParseText( fileContent, NULL );

	if( verbose )		
		std::cout << "End processing file:\t" << (basedir+"/"+file) << std::endl;	
}

void Parser::ParseText( std::string text, Entry* parent, std::string visibility )
{
	MatchResult matchResult = MatchResult( false, text, std::vector< std::string> () );

	//
	//	Match commentaries with // and /*
	//  TODO: block commentaries
	// 
	while( (matchResult = FindMatch( matchResult.TextUpdated(), "//([^\n]*?)" )).WasMatch() 
		|| (matchResult = FindMatch( matchResult.TextUpdated(), "/\\*([^\\*]*?)\\*/" )).WasMatch() )
	{
		//std::cout << "/* " << matchResult.SubMatches()[0] << " */" << std::endl << std::endl;
	}

	//
	//	Match for include local files, to process it
	//
	while( (matchResult = FindMatch( matchResult.TextUpdated(), "\\#include[ ]+\\\"(\\w*.\\w*)\\\"" )).WasMatch() )
	{		
		ParseFile( matchResult.SubMatches()[0] );
	}

	//
	//	Match preprocessor directives (#)
	// 
	while( (matchResult = FindMatch( matchResult.TextUpdated(), "#([^\n]*?)" )).WasMatch() )
	{
		//std::cout << "# " << matchResult.SubMatches()[0] << std::endl << std::endl;
	}	

	//
	//	Match for classes, and parse inside for declared functions, and inside class parenhies { - which are connected with in class definitions
	//	TODO: parent parametr for classes declared in classes
	// 
	EntryType matchType = NONE;	
	while( 
		   ((matchResult = FindMatch( matchResult.TextUpdated(), "class\\s+(\\w+)\\s*\\{([^{}]*)\\}\\s*;" )).WasMatch() && (matchType=CLASS) )
		|| ((matchResult = FindMatch( matchResult.TextUpdated(), "(((\\w+)\\s+)*?)*((\\w+)\\s*::)*\\s*(\\*?&?\\w+)\\s*\\(([^;:.\\)]*?)\\)(\\s*:\\s*(([^\\({}]*)\\s*\\(\\s*([^\\){}]*)\\s*\\)\\s*,*)+)?\\s*\\{([^{}]*)\\}" )).WasMatch() && (matchType=DEFINITION) )
		|| ((matchResult = FindMatch( matchResult.TextUpdated(), "(((\\w+)\\s+)*?)*(\\w+)\\s*\\(([^;.]*)\\)\\s*;" )).WasMatch() && (matchType=DECLARATION) )
		|| ((matchResult = FindMatch( matchResult.TextUpdated(), "\\{([^{}]*)\\}\\s*;" )).WasMatch() && (matchType=NONE) )
		|| ((matchResult = FindMatch( matchResult.TextUpdated(), "\\{([^{}]*)\\}" )).WasMatch() ) && (matchType=NONE) )
	{
		switch( matchType ) 
		{
			case CLASS : 
			{
				if( verbose ) 
					std::cout << "\tFound class:\t" << matchResult.SubMatches()[0] << std::endl;
				Entry *newClass = new Entry( matchResult.SubMatches()[0], CLASS, parent );
				filo.push_back( newClass );				
				ParseText( matchResult.SubMatches()[1], newClass );
			}
			break;
			case NONE :
			break;
			case DEFINITION :
			{
					
				MatchResult matchDefinition = MatchResult( false, matchResult.GetWholeMatch(), std::vector< std::string> () );
				if( (matchDefinition = FindMatch( matchResult.GetWholeMatch(), "(((\\w+)\\s+)*?)*((\\w+)\\s*::)*\\s*(\\*?&?\\w+)\\s*\\(([^;:.{}\\)]*?)\\)" )).WasMatch() ) 
				{
					if( verbose )
					{
						std::cout << "\t\tFound method definition:\t";
						std::cout << (( matchDefinition.SubMatches().size() > 3 ) ? matchDefinition.SubMatches()[0] : "");
					}
					// Find class specification
					std::string parentOf = "";
;						for( unsigned int i = 0; i < matchDefinition.SubMatches().size(); i++ ) {
						if( matchDefinition.SubMatches()[i].find("::") != std::string::npos )
						{
							if( verbose ) std::cout << matchDefinition.SubMatches()[i+1]+"::";
							parentOf = matchDefinition.SubMatches()[i+1];
						}
					}
					if( verbose ) std::cout << matchDefinition.SubMatches()[matchDefinition.SubMatches().size()-2] << " (" << matchDefinition.SubMatches()[matchDefinition.SubMatches().size()-1] << ")" << std::endl;

					Entry * newDefinition = new Entry( 	trim(( matchDefinition.SubMatches().size() > 3 ) ? matchDefinition.SubMatches()[0] : ""),
														matchDefinition.SubMatches()[matchDefinition.SubMatches().size()-2],
														matchDefinition.SubMatches()[matchDefinition.SubMatches().size()-1],
														DEFINITION, NULL );
					if( UpdateDefinition( newDefinition, parentOf ) )
						delete newDefinition;
					else
						filo.push_back( newDefinition );
				}
			}
			break;
			case DECLARATION :
			{
				if( matchResult.SubMatches()[matchResult.SubMatches().size()-2] == "if" )
					break;
				
				if( verbose )
				{	
					std::cout << "\t\tFound method declaration:\t" << ((matchResult.SubMatches().size()>3)?matchResult.SubMatches()[0]:"");
					std::cout << (( parent != NULL ) ? parent->GetName()+"::" : "");
					std::cout << matchResult.SubMatches()[matchResult.SubMatches().size()-2] << " (" << matchResult.SubMatches()[matchResult.SubMatches().size()-1] << ")" << std::endl;
				}
				Entry *newMethod = new Entry( trim((matchResult.SubMatches().size()>3)?matchResult.SubMatches()[0]:""), matchResult.SubMatches()[matchResult.SubMatches().size()-2], matchResult.SubMatches()[matchResult.SubMatches().size()-1], DECLARATION, parent ); 
				filo.push_back( newMethod );
			}
			break;
			case NAMESPACE :
			break;				
		}		

	}
}

bool Parser::UpdateDefinition( Entry *value, std::string parentOf )
{
	Entry *parent = NULL;
	Entry *declaration = NULL;
	for( std::deque< Entry* >::iterator i = filo.begin(); i != filo.end(); i++ )
	{
		if( (*i)->GetName() == parentOf )
			parent = (*i);
		if( (*i)->GetName() == value->GetName() && (*i)->GetArguments() == value->GetArguments() && (*i)->GetReturnType() == value->GetReturnType() )
			declaration = (*i);
	}
	
	
	if( declaration == NULL )
	{
		return true;
	}
	else
	{
		declaration->SetDefinition( true );
		return true;
	}
}

std::map< std::string, std::string > Parser::PrepareVariables( void )
{
	std::map< std::string, std::string > variables;
	
	std::string allows;
	std::string stubs;
	
	while( filo.size() )
	{
		Entry *entry = filo.front();
		filo.pop_front();
		if( entry->GetType() == CLASS )
		{
			allows += "//reflection->allow(\".*::"+entry->GetName()+".*\");\n";
		}
		if( entry->GetType() == DECLARATION && !entry->HasDefinition() && entry->HasParent() )
		{
			stubs += "stub(\""+entry->GetName()+"\","+entry->GetReturnType()+","+entry->GetParentName()+","+entry->GetName()+","+entry->GetArguments()+");\n";
		}
	}
		
	variables.insert( std::make_pair< std::string, std::string >( "allows", allows ) );
	variables.insert( std::make_pair< std::string, std::string >( "stubs", stubs ) );

	return variables;
}

MatchResult Parser::FindMatch( std::string text, std::string expression, bool doDelete )
{
	MatchResult matchResult = MatchResult();
	matchResult.setText( text );
	

	regex_t rx;
    regmatch_t *matches;

	int result = regcomp( &rx, expression.c_str(), REG_EXTENDED );
	if (result != 0) 
	{
		char buf[256];
		
		regerror( result, &rx, buf, sizeof(buf) );
		fprintf( stderr, "Invalid regular expression: %s\n", buf );
		
		regfree( &rx );

		return matchResult;
	}

	matches = (regmatch_t *)malloc( (rx.re_nsub + 1) * sizeof(regmatch_t) );

	result = 0;
	result = regexec( &rx, text.c_str(), rx.re_nsub + 1, matches, 0 );

	if (!result) {
		/*if( verbose ) fprintf( stderr, "Match %2d from %2d to %2d: \"%s\"\n",
				                 0, matches[0].rm_so, matches[0].rm_eo,
				                 text.substr( matches[0].rm_so, matches[0].rm_eo-matches[0].rm_so ).c_str() );*/
		for (unsigned int i = 1; i <= rx.re_nsub; i++) {
			  if (matches[i].rm_so != -1) {
				    /*if( verbose ) fprintf( stderr, "Match %2d from %2d to %2d: \"%s\"\n",
				                 i, matches[i].rm_so, matches[i].rm_eo,
				                 text.substr( matches[i].rm_so, matches[i].rm_eo-matches[i].rm_so ).c_str() );*/
					matchResult.addMatch( text.substr( matches[i].rm_so, matches[i].rm_eo-matches[i].rm_so ) );
			  }
		}

		
		matchResult.setMatch( true );				
		matchResult.setRange( matches[0].rm_so, matches[0].rm_eo );
		matchResult.AddWholeMatch( text.substr( matches[0].rm_so, matches[0].rm_eo-matches[0].rm_so ) );
		if( doDelete ) matchResult.setText( text.erase( matches[0].rm_so, matches[0].rm_eo-matches[0].rm_so ) );
	}
	

	free( matches );
	regfree( &rx );
	return matchResult;
}

MatchResult::MatchResult()
: wasMatch( false ) 
{
}

MatchResult::MatchResult( bool wasMatch, std::string textUpdated, std::vector< std::string > subMatches )
: wasMatch( wasMatch ), textUpdated( textUpdated ), subMatches( subMatches )
{
}

void MatchResult::addMatch( std::string match ) 
{ 
	subMatches.push_back( match ); 
}

void MatchResult::setText( std::string text ) 
{ 
	textUpdated = text; 
}

void MatchResult::setMatch( bool match ) 
{ 
	wasMatch = match; 
}
	
bool MatchResult::WasMatch() 
{ 
	return wasMatch; 
}

std::string MatchResult::TextUpdated() 
{ 
	return textUpdated; 
}

std::vector< std::string > MatchResult::SubMatches() 
{ 
	return subMatches; 
}

int MatchResult::getStart()
{
	return start;
}

int MatchResult::getEnd()
{
	return end;
}

void MatchResult::setRange( int start, int end )
{
	this->start = start;
	this->end = end;
}

std::string MatchResult::GetWholeMatch()
{
	return wholeMatch;
}

void MatchResult::AddWholeMatch(std::string wholeMatch)
{
	this->wholeMatch = wholeMatch;
}
