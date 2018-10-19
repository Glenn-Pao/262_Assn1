#pragma once
#ifndef HASHFUNCTION_H
#define HASHFUNCTION_H

#include "md5.h"
#include <string>
class HashFunction
{
protected:
	std::string _name;
	//name of function
	HashFunction(std::string name);
public:
	~HashFunction();

	//hash the password
	virtual std::string hash(const std::string& pwd) const = 0;

	//return name of password
	std::string name();
};
#endif