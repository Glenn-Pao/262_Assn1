#include "HashFunction.h"

//constructor, overloaded
HashFunction::HashFunction(std::string name) 
	: _name(name) 
{
}
//destructor
HashFunction::~HashFunction()
{
}
//return name of password
std::string HashFunction::name()
{
	return _name;
}


