#ifndef TUT_GEN_ENTRY
#define TUT_GEN_ENTRY

#include <iostream>

enum EntryType { NONE = 1, NAMESPACE = 2, CLASS = 3, DEFINITION = 4, DECLARATION = 5 };

class Entry
{
public:
	Entry(std::string name, EntryType type, Entry *parent, std::string visibility = "NOTAPPLICABLE");
	Entry(std::string rettype, std::string name, std::string arguments, EntryType type, Entry *parent, std::string visibility = "NOTAPPLICABLE");
	~Entry();

	std::string GetName();
	std::string GetArguments();
	std::string GetReturnType();
	std::string GetParentName();
	EntryType	GetType();

	bool		HasDefinition();
	bool 		HasParent();

	void		SetDefinition( bool definition );
private:
	std::string name;
	EntryType type;
	Entry *parent;
	std::string visibility;

	std::string rettype;
	std::string arguments;
	bool		definition;
};

#endif
