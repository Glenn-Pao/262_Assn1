#pragma once
#ifndef TABLECONFIG_H
#define TABLECONFIG_H

#include "Keys.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

class TableConfig
{
private:
	//establish the container for the hash-password
	vector<Keys> theKeys;

	//to find the index based on the hash provided
	int findIndex(const string& hash) const;
public:
	TableConfig();
	~TableConfig();

	//find a hash in the table and return the corresponding password
	vector<string> find(const string& hash) const;

	//add a new hash entry
	void insertEntry(const Keys& key);

	//alternate method to inser pair at end of table. Used when loading from file to reduce loading time
	void insertEntryAtEnd(const string& hash, const string& pwd);

	//print the table content
	ostream& print(ostream& stream) const;

	void freeMem();
};

#endif // !TABLECONFIG_H



