#pragma once
#ifndef KEYS_H
#define KEYS_H

#include <string>

using namespace std;
class Keys
{
private:
	string pwd;
	string hashVal;

public:
	Keys(const string& pwd, const string& hashVal);
	~Keys();

	string getPwd() const;
	string getHash() const;
};
#endif