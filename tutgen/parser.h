#ifndef TUT_GEN_PARSER
#define TUT_GEN_PARSER

#include "entry.h"

#include <vector>
#include <deque>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <regex.h>

class MatchResult 
{
public:
	MatchResult();
	MatchResult( bool wasMatch, std::string textUpdated, std::vector< std::string > subMatches = std::vector< std::string >() );

	void addMatch( std::string match );
	void setText( std::string text );
	void setMatch( bool match );
	void setRange( int start, int end );
		
	bool WasMatch();
	std::string TextUpdated();
	std::vector< std::string > SubMatches();
	std::string GetWholeMatch();
	void 		AddWholeMatch(std::string wholeMatch);

	int getStart();
	int getEnd();
private:
	bool wasMatch;
	std::string textUpdated;
	std::string wholeMatch;
	std::vector< std::string > subMatches;

	int start;
	int end;
};

class Parser 
{
public:
	Parser( bool verbose);
	~Parser();

	void Parse( std::string directory );
	std::map< std::string, std::string > PrepareVariables( void );

	static MatchResult FindMatch( std::string text, std::string expression, bool doDelete = true );

private:
	void ParseFile( std::string file );
	void ParseText( std::string text, Entry* parent, std::string visibility = "NOTAPPLICABLE" );

	bool UpdateDefinition( Entry *value, std::string parent );

	std::vector< std::string > 	files;
	std::vector< Entry* > 		entries;

	std::deque< Entry* > 		filo;

	bool verbose;
	std::string basedir;

};

#endif
