#include "RainbowTables.h"
#include <stdlib.h>		
#include <iostream>		
#include <cstring>		
#include <fstream>		
#include <string>
#include <omp.h>		//using OpenMP for parallelism
#include <sstream>
#include <bitset>
#include "TableConfig.h"	
#include "MD5Hashing.h"
using namespace std;
static const char CONSTCA_HASHCHAR[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

static int N_HEXAPOSITION[16];

const static char CONST_C_RAINBOWTABLEDELIMETER = ':';
const static int CONST_N_NUMOFCHARFORHASH = 32;					//Const static int to contain the number of characters for md5 hash
RainbowTables::RainbowTables(string const& fileName, string const& rainbowTableName)
{
	//establish the MD5 Hashing and ALL possible alphanumeric characters
	theHash = new MD5Hashing();
	chars = "a-zA-Z0-9";
	replaceString(chars, "a-z", LettersLower);
	replaceString(chars, "A-Z", LettersUpper);
	replaceString(chars, "0-9", Digits);
	theTable = TableConfig();
	init(fileName, rainbowTableName);
}
RainbowTables::~RainbowTables()
{
	delete theHash;
	theTable.freeMem();
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
	double numChars = (double)reductionBitLen / 4.0;

	reductionCharLen = ceil(numChars);
}
int RainbowTables::searchUnusedPassword()
{
	for (int i = 0; i < thePasswords.size(); i++)
	{
		if (!thePasswords[i].getUsedState())
			return i;
	}
}
int RainbowTables::convertStringIntoInt(string theString)
{
	//Declare string stream to convert string into int
	stringstream ss;
	//Store string into stringstream variable
	ss << theString;

	//Declare int to return
	int theInt;
	//Store converted string into int
	ss >> theInt;

	//Return int
	return theInt;
}
string RainbowTables::convertCharIntoString(char theChar)
{
	//Declare string stream to convert char into string
	stringstream ss;
	//Store char into stringstream variable
	ss << theChar;

	//Declare string to return
	string theString;
	//Store converted char into int
	ss >> theString;

	//Return string
	return theString;
}
//calculate binary value
int RainbowTables::calcReductionBin(string hashVal)
{
	string bin;
	/*unsigned char* bytes = new unsigned char[16];
	int temp;*/
	for (int i = 0; i < reductionCharLen; i++)
	{
		/*sscanf(hashVal.c_str() + 2 * i, "%2x", &temp);
		bytes[i] = temp;*/
		//Declare string stream to convert character into a hexadecimal character
		stringstream ss;
		ss << hex << hashVal[i];
		//Assign hexa charaacter into a unsigned int
		unsigned n;
		ss >> n;
		//Convert int into binary
		bitset<4> b(n);
		//Append binary value to that of the string
		bin = (string)bin + (string)b.to_string();
	}
	cout << bin << endl;
	//get the bits needed
	string bitsNeeded;

	for (int i = 0; i < reductionBitLen; i++)
	{
		bitsNeeded = (string)bitsNeeded + (string)convertCharIntoString(bin[i]);
	}

	//convert into int
	int n_bin = convertStringIntoInt(bitsNeeded);
	cout << "bits needed: " << n_bin << endl;
	return n_bin;
}
int RainbowTables::calcReductionPwdNum(int val)
{
	int n_binaryNumber = val;

	int n_dec = 0;
	long l_rem = 0;
	long l_base = 1;

	while (n_binaryNumber > 0)
	{
		l_rem = n_binaryNumber % 10;
		n_dec = n_dec + l_rem * l_base;
		l_base = l_base * 2;
		n_binaryNumber = n_binaryNumber / 10;
	}

	return n_dec;
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
	//Standard bubble sort

	//Traverse through the chains that exist
	for (int i = 0; i < theChains.size(); i++)
	{
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
	ofstream ofs_rainbowFile;
	//Declare string to append password and hash to rainbow.txt
	string s_chain;

	//Attach rainbow.txt character delimeter to the end of chain starting password
	string s_rainbowDelimeter = convertCharIntoString(CONST_C_RAINBOWTABLEDELIMETER);
	s_chain = (string)thePwd + (string)s_rainbowDelimeter + (string)theHash;

	//Open new text file & append to it (Create if it does not exist)
	ofs_rainbowFile.open("Rainbow.txt", ios::app);
	//Append content into file
	ofs_rainbowFile << s_chain << endl;
	//Close new text file
	ofs_rainbowFile.close();
}
void RainbowTables::writeRainbowTextFile()
{
	//Declare an int to indicate the current hexa character (0 by default)
	int n_hexaChar = 0;
	//Set the first character hexa mark to be 0
	N_HEXAPOSITION[n_hexaChar] = 0;

	//Traverse through the number of chain
	for (int i = 0; i < theChains.size(); i++)
	{
		//Check if the first character is not equal to that of the hexa char
		if (theChains[i].getChainLastHash()[0] != CONSTCA_HASHCHAR[n_hexaChar])
		{
			//Traverse through the hexa character list
			for (int a = 0; a < 16; a++)
			{
				//Check if which character the hexa character is for the hash
				if (theChains[i].getChainLastHash()[0] == CONSTCA_HASHCHAR[a])
				{
					//Set the current hexa char to be that
					n_hexaChar = a;
					//Mark the position in N_HEXAPOSITION
					N_HEXAPOSITION[n_hexaChar] = i;
					//Break out of loop
					break;
				}
			}
		}
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
	else if (theInput.length() != CONST_N_NUMOFCHARFORHASH)
	{
		//Print error message
		cout << "Hash value does not contain 32 characters" << endl;
		return false;
	}

	//Traverse through string and check if it contains any characters aside from those allowed
	for (int i = 0; i < theInput.length(); i++)
	{
		//Declare bool to state if character is valid (False by default)
		bool validChar = false;

		//Traverse through list of allowed characters
		for (int a = 0; a < 16; a++)
		{
			//Check if character is equal to the character
			if (theInput[i] == CONSTCA_HASHCHAR[a])
			{
				//Set valid char to be true
				validChar = true;
				break;
			}
		}

		//Check if char is invalid
		if (validChar != true)
		{
			//Print error message
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
		//Convert to lower
		theString[i] = tolower(theString[i]);
	}
	return theString;
}
int RainbowTables::calcStartingIndexPos(string hashVal)
{
	//search for the starting point
	for (int i = 0; i < 16; i++)
	{
		if (hashVal[0] == CONSTCA_HASHCHAR[i])
		{
			return N_HEXAPOSITION[i];
		}
	}
}
vector<int> RainbowTables::calcListOfPossibleChains(string theInput)
{
	//Declare vector to store list of possible chavoid addChainToRainbowTableList(string _sChainPassword, string _sChainHash)in
	vector<int> possibleChains;
	//Bool to state if hash value is found at end of chains (False by default)
	bool b_hashValueAtEnd = false;

	//Declare int to indicate starting index for searching
	int startIndex = calcStartingIndexPos(theInput);

	//Traverse through the rainbow table & check if any of the hashes matches the chain last hash
	for (int a = startIndex; a < theChains.size(); a++)
	{
		//If hash value matches
		if (theInput == theChains[a].getChainLastHash())
		{
			//Push back into list of possible chains
			possibleChains.push_back(a);
			//Set bool of hash value found at end of chains to be true
			b_hashValueAtEnd = true;
		}
	}

	//If hash at end of chains is true
	if (b_hashValueAtEnd == true)
	{
		//Return vector of possible lines
		return possibleChains;
	}

	//Declare string to store the password
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

		//Traverse through the rainbow table & check if any of the hashes matches the chain last hash
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
	//Return vector of possible lines
	return possibleChains;
}
string RainbowTables::searchPasswordFromChain(string matchingHash, vector<int> vecPossibleChain)
{
	//Declare string to hold the last reduced password
	string reducedPassword;
	//Declare string to hold the last hashed password
	string hashed;
	//Declare string to state if the password has been found (False by default)
	bool found = false;

	//Traverse through list of possible chains
	for (int i = 0; i < vecPossibleChain.size(); i++)
	{
		//Set reduced password to be the chain starting password
		reducedPassword = theChains[vecPossibleChain[i]].getChainStartPassword();

		cout << "Reduced password: " << reducedPassword << endl;

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
	if (found == false)
	{
		return "Invalid hash value";
	}
}
void RainbowTables::main()
{
	while (1)
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

	//sort the chains
	sortChainHashes();

	writeRainbowTextFile();

	cout << "Complete setup" << endl;
}
void hexconversion(const char *text, unsigned char* bytes)
{
	//implementation is MD5
	int temp;
	for (int i = 0; i < 16; i++)
	{
		sscanf(text + 2 * i, "%2x", &temp);
		bytes[i] = temp;
	}
}
string RainbowTables::hashTargetWord(const string& pwd) const
{
	return theHash->hash(pwd);
}
void RainbowTables::replaceString(string& theString, const string& from, const string& to)
{
	// Find string <from>.
	size_t start_pos = theString.find(from);
	if (start_pos != std::string::npos)
	{
		// If found, replace sequence to <to>.
		theString.replace(start_pos, from.length(), to);
	}
}