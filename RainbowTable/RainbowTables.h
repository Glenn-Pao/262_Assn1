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
	int calcStartingIndexPos(string hashVal);
	int convertStringIntoInt(string theString);
	string convertCharIntoString(char theChar);

	void sortChainHashes();
	void writeRainbowTextFile();
	void appendRainbowTextFile(string thePwd, string theHash);

	//initialize table
	void init(const string& fileName, const string& rainbowTableName);

	void replaceString(string& theString, const string& from, const string& to);
	string convertToLowerCase(string theString);
	bool verifyHashInput(string theInput);
	vector<int> calcListOfPossibleChains(string theInput);
	string searchPasswordFromChain(string matchingHash, vector<int> vecPossibleChain);
public:
	RainbowTables(string const& fileName, string const& rainbowTableName);
	~RainbowTables();
	string hashTargetWord(const string& pwd) const;
	void main();
};
#endif // !RAINBOWTABLE_H