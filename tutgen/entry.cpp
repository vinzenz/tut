#include "entry.h"

Entry::Entry(std::string name, EntryType type, Entry *parent, std::string visibility)
: name(name), type(type), parent(parent), visibility(visibility), definition(false)
{
}

Entry::Entry(std::string rettype, std::string name, std::string arguments, EntryType type, Entry *parent, std::string visibility)
: name(name), type(type), parent(parent), visibility(visibility), rettype(rettype), arguments(arguments), definition(false)
{
	
}

Entry::~Entry()
{
}

std::string Entry::GetName() 
{ 
	return name; 
}

EntryType Entry::GetType()
{
	return type;
}

bool Entry::HasDefinition()
{
	return definition;
}

bool Entry::HasParent()
{
	return (parent!=NULL);
}

std::string Entry::GetArguments()
{ 
	return arguments; 
}

std::string Entry::GetReturnType()
{ 
	return rettype; 
}

std::string Entry::GetParentName()
{ 
	return (parent==NULL)?"":parent->name; 
}

void Entry::SetDefinition( bool definition )
{
	this->definition = definition;
}

