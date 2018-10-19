#ifndef MD5HASHING_H
#define MD5HASHING_H
#pragma once
#include "HashFunction.h"
#include <string>

using namespace std;
class MD5Hashing : public HashFunction
{
public:
	MD5Hashing();
	~MD5Hashing();

	virtual string hash(const string& pwd) const;
};
#endif // !MD5HASHING_H