#pragma once
#ifndef RAINBOWTABLE_H
#define RAINBOWTABLE_H
#include <string>
#include <vector>
#include "HashFunction.h"
#include "HashChain.h"
#include "Password.h"

class RainbowTables
{
private:
	int reductionBitLen;		//the reduction bit length
	int reductionCharLen;		//the reduction character length, assumed to be 32 bits
	HashFunction* theHash;		//the hash method, MD5

	//unused and total number of passwords
	int unusedPasswords;
	int totalPasswords;

	//the password vector
	std::vector<Password> thePasswords;
	//the chain vector
	std::vector<HashChain> theChains;

	//record all passwords
	void recordPasswords(const string& fileName);
	//setup rainbow table
	void initRainbowTable(const string& rainbowTableName);
	//search for unused passwords in the table
	int	 searchUnusedPassword();
	//set the bit length
	void setBitLen();
	//set the character length
	void setCharLen();
	//create the hash chain
	void createTheHashChain();
	//do the reduction function
	int reduction(string hashPassword);
	//calculate the reduction binary value
	int calcReductionBin(string hashVal);
	//calculate the reduction password index
	int calcReductionPwdNum(int val);
	//calculate password line number
	int calcPwdLineNum(int val);
	//calculate the start index position
	int calcStartingIndexPos(string hashVal);
	//convert string to int
	int convertStringIntoInt(string theString);
	//convert char into string
	string convertCharIntoString(char theChar);
	//sort the chain hashes
	void sortChainHashes();
	//write to rainbow.txt
	void writeRainbowTextFile();
	//append entries to rainbow.txt
	void appendRainbowTextFile(string thePwd, string theHash);

	//initialize table, main function of class
	void init(const string& fileName, const string& rainbowTableName);
	//verify the hash input
	bool verifyHashInput(string theInput);
	//convert the string to all lower case
	string convertToLowerCase(string theString);
	//return a vector of possible chains using password ID
	vector<int> calcListOfPossibleChains(string theInput);
	//search the password using the hash and possible chains
	string searchPasswordFromChain(string matchingHash, vector<int> vecPossibleChain);
public:
	//constructor/destructor
	RainbowTables(string const& fileName, string const& rainbowTableName);
	~RainbowTables();

	//hash the word
	string hashTargetWord(const string& pwd) const;
	
	//main handler
	void main();
};
#endif // !RAINBOWTABLE_H