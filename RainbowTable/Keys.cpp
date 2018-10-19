#include "Keys.h"

//overloaded constructor that requires the password and hash value
Keys::Keys(const string& pwd, const string& hashVal)
{
	this->pwd = pwd;
	this->hashVal = hashVal;
}

Keys::~Keys()
{
}

string Keys::getPwd() const
{
	return pwd;
}
string Keys::getHash() const
{
	return hashVal;
}