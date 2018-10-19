#include "RainbowTables.h"
#include <stdlib.h>		
#include <iostream>		
#include <cstring>		
#include <fstream>		
#include <string>
#include <omp.h>		//using OpenMP for parallelism
#include <sstream>
#include <bitset>
#include "MD5Hashing.h"
using namespace std;
static const char HASHCHARS[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

static int HEXAPOS[16];

const static char DELIMITER = ':';
const static int NUMHASH = 32;					//Const static int to contain the number of characters for md5 hash
RainbowTables::RainbowTables(string const& fileName, string const& rainbowTableName)
{
	//establish the MD5 Hashing and ALL possible alphanumeric characters
	theHash = new MD5Hashing();
	init(fileName, rainbowTableName);
}
RainbowTables::~RainbowTables()
{
	delete theHash;
}
void RainbowTables::initRainbowTable(const string& rainbowTableName)
{
	//remove the previous text file
	remove(rainbowTableName.c_str());

	//create a new file
	ofstream newFile;

	//create and close the file
	newFile.open(rainbowTableName.c_str(), ios::app);
	newFile.close();
}
void RainbowTables::recordPasswords(const string& fileName)
{
	//parsing variable
	string line;
	//load up the target file
	ifstream file(fileName);

	//if the file is found
	if (file)
	{
		//increment the count with every line is detected
		while (getline(file, line))
		{
			unusedPasswords++;

			//declare the new password
			Password tempPass(line);

			//store password
			thePasswords.push_back(tempPass);
		}
		//close file
		file.close();

		totalPasswords = unusedPasswords;
		cout << "Passwords recorded!" << endl;
	}
	else
	{
		cout << "File not found!" << endl;
	}
}
void RainbowTables::setBitLen()
{
	//compute the password length bits
	reductionBitLen = ceil(log2(totalPasswords));
}
void RainbowTables::setCharLen()
{
	//this was assuming the bit length is only 4. it's not the case but let's just work with this
	double numChars = (double)reductionBitLen / 4.0;

	reductionCharLen = ceil(numChars);
}
int RainbowTables::searchUnusedPassword()
{
	//find all unused passwords
	for (int i = 0; i < thePasswords.size(); i++)
	{
		if (!thePasswords[i].getUsedState())
			return i;
	}
}
int RainbowTables::convertStringIntoInt(string theString)
{
	stringstream ss;
	ss << theString;

	//Declare int to return
	int theInt;
	ss >> theInt;

	//Return int
	return theInt;
}
string RainbowTables::convertCharIntoString(char theChar)
{
	stringstream ss;
	ss << theChar;

	//Declare string to return
	string theString;
	ss >> theString;

	//Return string
	return theString;
}
//calculate binary value
int RainbowTables::calcReductionBin(string hashVal)
{
	string bin;
	for (int i = 0; i < reductionCharLen; i++)
	{
		stringstream ss;
		ss << hex << hashVal[i];
		//Assign hexa charaacter into a unsigned int
		unsigned n;
		ss >> n;
		//change int into binary
		bitset<12> b(n);
		//append to a string
		bin = (string)bin + (string)b.to_string();
	}
	//get the bits needed
	string bitsNeeded;

	for (int i = 0; i < reductionBitLen; i++)
	{
		bitsNeeded = (string)bitsNeeded + (string)convertCharIntoString(bin[i]);
	}

	//convert into int
	int n_bin = convertStringIntoInt(bitsNeeded);
	//cout << "bits needed: " << n_bin << endl;
	return n_bin;
}
int RainbowTables::calcReductionPwdNum(int val)
{
	int binNum = val;

	int decimal = 0;
	long remainder = 0;
	long base = 1;

	while (binNum > 0)
	{
		remainder = binNum % 10;
		decimal = decimal + remainder * base;
		base = base * 2;
		binNum = binNum / 10;
	}

	return decimal;
}
int RainbowTables::calcPwdLineNum(int val)
{
	//Check if the number given is bigger than the number of password
	if (val >= totalPasswords)
	{
		//Return the result of the mod value of password line number by the number of possible password
		int n_passwordLineNumber = val % totalPasswords;

		//cout << "Value exceeds totalPasswords at " << val << ". Reducing the number to " << n_passwordLineNumber << endl;
		return n_passwordLineNumber;
	}
	//Else no need to wrap
	else
	{
		return val;
	}
}
int RainbowTables::reduction(string hashPassword)
{
	//Declare int to store the bits required to perform the reduction function
	int reductionBits = calcReductionBin(hashPassword);
	//Declare int to store the index of the password that is the product of the reduction function
	int reducedPwdIndex = calcReductionPwdNum(reductionBits);
	//Wrap the line number if it is too big
	reducedPwdIndex = calcPwdLineNum(reducedPwdIndex);

	//Return reduced password index
	return reducedPwdIndex;
}
void RainbowTables::createTheHashChain()
{
	//set up the start and end chain
	string start;
	string end;

	string chainPassword;

	//find unused passwords
	int firstPasswordIndex = searchUnusedPassword();
	//change used state to true
	thePasswords[firstPasswordIndex].setUsedState(true);
	unusedPasswords--;

	//init the chain password and starting hash points
	chainPassword = thePasswords[firstPasswordIndex].getPassword();
	start = chainPassword;

	for (int i = 0; i < 5; i++)
	{
		//declare hash value of password
		string hashPassword = hashTargetWord(chainPassword);
		int reducedPasswordIndex = reduction(hashPassword);

		if (!thePasswords[reducedPasswordIndex].getUsedState())
		{
			thePasswords[reducedPasswordIndex].setUsedState(true);
			unusedPasswords--;
		}

		chainPassword = thePasswords[reducedPasswordIndex].getPassword();
	}

	//hash one more time
	end = hashTargetWord(chainPassword);

	cout << "Start chain password: " << start << ", End chain hash: " << end << endl;
	//add to rainbow table list
	HashChain theChain(start, end);
	theChains.push_back(theChain);
}
void RainbowTables::sortChainHashes()
{
	//Standard bubble sort - do you really want to sort this out?
	for (int i = 0; i < theChains.size(); i++)
	{
		//cout << "Sorting " << theChains[i].getChainStartPassword() << endl;
		//Traverse through the chains that exist minus one
		for (int a = 0; a < theChains.size() - 1; a++)
		{
			//Check if the existing hash value first character is higher than that of the next hash value
			if (theChains[a].getChainLastHash()[0] > theChains[a + 1].getChainLastHash()[0])
			{
				//Create a temp chain value
				HashChain tempChain(theChains[a].getChainStartPassword(), theChains[a].getChainLastHash());
				//Swap places
				theChains[a] = theChains[a + 1];
				theChains[a + 1] = tempChain;
			}
		}
	}
}
void RainbowTables::appendRainbowTextFile(string thePwd, string theHash)
{
	//Declare ofstream to write into rainbow text file
	ofstream rainbowFile;
	//Declare string to append password and hash to rainbow.txt
	string chain;

	//Attach rainbow.txt character delimeter to the end of chain starting password
	string rainbowDelimeter = convertCharIntoString(DELIMITER);
	chain = (string)thePwd + (string)rainbowDelimeter + (string)theHash;

	//just to check that it is doing what it is supposed to be doing, 
	//cout << s_chain << endl;

	//Open new text file & append to it (Create if it does not exist)
	rainbowFile.open("Rainbow.txt", ios::app);
	//Append content into file
	rainbowFile << chain << endl;
	//Close new text file
	rainbowFile.close();
}
void RainbowTables::writeRainbowTextFile()
{
	for (int i = 0; i < theChains.size(); i++)
	{
		//Append chain to rainbow.txt
		appendRainbowTextFile(theChains[i].getChainStartPassword(), theChains[i].getChainLastHash());
	}
}
bool RainbowTables::verifyHashInput(string theInput)
{
	//If character is the quit character
	if (theInput == "0")
	{
		cout << "Thank you for using the rainbow table, hope you found your pre-image!" << endl;
		return true;
	}
	//Check if it contains 32 character
	else if (theInput.length() != NUMHASH)
	{
		//Print error message
		cout << "Hash value does not contain 32 characters" << endl;
		return false;
	}

	for (int i = 0; i < theInput.length(); i++)
	{
		bool validChar = false;

		//Traverse through list of allowed characters
		for (int a = 0; a < 16; a++)
		{
			//Check if character is equal to the character
			if (theInput[i] == HASHCHARS[a])
			{
				//Set valid char to be true
				validChar = true;
				break;
			}
		}

		//Check if char is invalid
		if (!validChar)
		{
			cout << "Error, hash value contains invalid hexa characters" << endl;
			return false;
		}
	}

	//If code reaches here is valid
	return true;
}
string RainbowTables::convertToLowerCase(string theString)
{
	//change to lower case
	for (int i = 0; i < theString.length(); i++)
	{
		theString[i] = tolower(theString[i]);
	}
	return theString;
}
int RainbowTables::calcStartingIndexPos(string hashVal)
{
	//search for the starting point
	for (int i = 0; i < 16; i++)
	{
		if (hashVal[0] == HASHCHARS[i])
		{
			return HEXAPOS[i];
		}
	}
}
vector<int> RainbowTables::calcListOfPossibleChains(string theInput)
{
	//the temp vector of possible chains
	vector<int> possibleChains;
	//flag to check if hash is found
	bool found = false;

	//make a starting point
	int startIndex = calcStartingIndexPos(theInput);

	//Search through the vector
	for (int a = startIndex; a < theChains.size(); a++)
	{
		if (theInput == theChains[a].getChainLastHash())
		{
			//Push back into list of possible chains
			possibleChains.push_back(a);
			//Set bool of hash value found at end of chains to be true
			found = true;
		}
	}

	if (found)
	{
		//Return vector of possible lines
		return possibleChains;
	}

	string pwd;
	//Set password to store the hash value initially
	pwd = theInput;

	//Loop 5 times
	for (int i = 0; i < 5; i++)
	{
		//Reduce hash value
		int reducedPasswordIndex = reduction(pwd);

		//Store the reduced password
		pwd = thePasswords[reducedPasswordIndex].getPassword();

		//Hash the password
		pwd = hashTargetWord(pwd);

		//Set int to indicate starting index for searching
		startIndex = calcStartingIndexPos(pwd);

		//reduce the chances of collisions
		for (int a = startIndex; a < theChains.size(); a++)
		{
			//If hash value matches
			if (pwd == theChains[a].getChainLastHash())
			{
				//Push back into list of possible chains
				possibleChains.push_back(a);
			}
		}
	}
	return possibleChains;
}
string RainbowTables::searchPasswordFromChain(string matchingHash, vector<int> vecPossibleChain)
{
	string reducedPassword;
	string hashed;

	//did you find the password inside the chain?
	bool found = false;

	//Traverse through list of possible chains
	for (int i = 0; i < vecPossibleChain.size(); i++)
	{
		//Set reduced password to be the chain starting password
		reducedPassword = theChains[vecPossibleChain[i]].getChainStartPassword();

		//check that it is correct
		//cout << "Reduced password: " << reducedPassword << endl;

		//Hash and reduce 5 times
		for (int a = 0; a < 5; a++)
		{
			//Store the hash value of password
			hashed = hashTargetWord(reducedPassword);

			//Check to see if hash value matches thats in argument
			if (hashed == matchingHash)
			{
				//Set password found to be true
				found = true;
				//Return reduced password
				return reducedPassword;
			}

			//Reduce and store reduced password index
			int reducedPasswordIndex = reduction(hashed);
			//Store the current point of the password to be that of the next password to hash
			reducedPassword = thePasswords[reducedPasswordIndex].getPassword();
		}

		//Hash the last time
		hashed = hashTargetWord(reducedPassword);

		//Check if the last hash matches hash value to be matched
		if (hashed == matchingHash)
		{
			//Return reducedPassword
			return reducedPassword;
		}
	}

	//If password not found
	if (!found)
	{
		return "Invalid hash value";
	}
}
void RainbowTables::main()
{
	while (true)
	{
		//Prompt user for user input
		cout << "Enter a hash value to get password or '0' to quit program." << endl;
		string hashInput;
		getline(cin, hashInput);

		//Convert user input into lower case
		hashInput = convertToLowerCase(hashInput);
		if (verifyHashInput(hashInput) == true)
		{
			//Check if user wants to quit
			if (hashInput == "0")
			{
				break;
			}
			else
			{
				vector<int> listOfPossibleChains = calcListOfPossibleChains(hashInput);
				string finalProduct = searchPasswordFromChain(hashInput, listOfPossibleChains);
				//Check if invalid hash has been entered
				if (finalProduct == "Invalid hash value")
				{
					cout << finalProduct << ", hash value does not correspond to any password, please try again." << endl;
				}
				else
				{
					cout << "Pre-Image is : " << finalProduct << endl;
				}
			}
		}
	}
}
void RainbowTables::init(const string& fileName, const string& rainbowTableName)
{
	//initialize the rainbow table
	initRainbowTable(rainbowTableName);

	//record the passwords from text file
	recordPasswords(fileName);

	//set the bit length for reduction
	setBitLen();

	//set the characters for reduction
	setCharLen();

	//create the hash chain
	while (unusedPasswords > 0)
	{
		//create all of the hash chains
		createTheHashChain();
	}

	//sort the chains..it took way too long so i commented it out
	//sortChainHashes();

	writeRainbowTextFile();

	cout << "Complete setup" << endl;
}
string RainbowTables::hashTargetWord(const string& pwd) const
{
	return theHash->hash(pwd);
}