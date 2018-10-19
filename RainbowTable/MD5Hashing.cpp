#include "MD5Hashing.h"



MD5Hashing::MD5Hashing()
	: HashFunction("md5")
{
}

MD5Hashing::~MD5Hashing()
{
}

string MD5Hashing::hash(const string& pwd) const
{
	return md5(pwd);
}