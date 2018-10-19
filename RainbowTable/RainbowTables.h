#pragma once
#ifndef RAINBOWTABLE_H
#define RAINBOWTABLE_H
#include <string>
#include <vector>
#include "HashFunction.h"
#include "TableConfig.h"
#include "HashChain.h"
#include "Password.h"

#define LettersLower "abcdefghijklmnopqrstuvwxyz"
#define LettersUpper "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define Digits "0123456789"

class RainbowTables
{
private:
	int columns;			//column
	int rows;				//row
	string chars;			//the characters
	int reductionBitLen;		//the password length
	int reductionCharLen;
	TableConfig theTable;	//table containing hash and passwords
	HashFunction* theHash;	//the hash method, MD5

	int unusedPasswords;
	int totalPasswords;

	std::vector<Password> thePasswords;
	std::vector<HashChain> theChains;

	void recordPasswords(const string& fileName);
	void initRainbowTable(const string& rainbowTableName);
	int	 searchUnusedPassword();
	void setBitLen();
	void setCharLen();
	void createTheHashChain();
	int reduction(string hashPassword);
	int calcReductionBin(string hashVal);
	int calcReductionPwdNum(int val);
	int calcPwdLineNum(int val);


	//initialize table
	void init(const string& fileName, const string& rainbowTableName);

	string reduction(const string& hash, int column) const;
	string createHashChain(string password) const;
	string getLastHash(const string& startHash, int startCol) const;
	string findHashInsideChain(const string& startingPassword, const string& startingHash) const;
	void replaceString(string& theString, const string& from, const string& to);
public:
	RainbowTables(int col, int row, string const& fileName, string const& rainbowTableName);
	~RainbowTables();

	void writeFile(const string& fileNmae) const;
	void readFile(const string& fileName);

	string hashTargetWord(const string& pwd) const;
	string checkPassword(const string& theHash) const;
	string testHashReduce(const string& testWord) const;
};
#endif // !RAINBOWTABLE_H