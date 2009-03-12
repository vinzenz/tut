#ifndef TUT_GEN_CONFIG
#define TUT_GEN_CONFIG

#include <map>
#include <vector>
#include <string>
#include <fstream>

class Config 
{
public:
	Config();
	~Config();

	bool GenerateFromConfig( std::string fileIn, std::string fileOut, std::string tag, std::map< std::string, std::string> values );
	std::vector< std::string >  GetValue( std::string file, std::string value );
	
	void PrepareDirectory( std::string directory );
private:
	
};


#endif
