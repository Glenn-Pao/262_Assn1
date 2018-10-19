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

RainbowTables::RainbowTables(int col, int row, string const& fileName, string const& rainbowTableName)
{
	//establish the MD5 Hashing and ALL possible alphanumeric characters
	theHash = new MD5Hashing();
	columns = col;
	rows = row;
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
		if (!thePasswords[i].getUsedState)
			return i;
	}
}
//calculate binary value
int RainbowTables::calcReductionBin(string hashVal)
{
	string bin;
	for (int i = 0; i < reductionCharLen; i++)
	{
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

	//get the bits needed
	string bitsNeeded;

	for(int i = 0; i < reductionBitLen; i++)

	//convert into int
	int n_bin = convertStringIntoInt()
}
int RainbowTables::calcReductionPwdNum(int val)
{

}
int RainbowTables::calcPwdLineNum(int val)
{

}
int RainbowTables::reduction(string hashPassword)
{

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
	}
}
void RainbowTables::init(const string& fileName, const string& rainbowTableName)
{
	initRainbowTable(rainbowTableName);
	recordPasswords(fileName);

	while (unusedPasswords > 0)
	{
		//create all of the hash chains
		createTheHashChain();
	}

	//password and hash
	string password, hash;

	//temporary vector 
	vector<Keys> theKeys;

	//parsing variable
	string line;
	//load up the target file
	ifstream file(fileName);

	//passwordLen = 25143;

	//cout << "File Name: " << fileName << endl;
	if (file)
	{
		omp_lock_t lock;
		omp_init_lock(&lock);

		//increment the count with every line is detected
		while (getline(file, line))
		{
			int threadNum = omp_get_thread_num();
			int totalThreadCnt = omp_get_num_threads();

			for (int i = threadNum; i < rows; i += totalThreadCnt)
			{
				//generate the corresponding hash to each password
				hash = createHashChain(line);

				//add to the temporary vector
				Keys thisKey(line, hash);
				theKeys.push_back(thisKey);

				//add the pair values into the temporary vector. if thread is idle, unlock it
				if (omp_test_lock(&lock))
				{
					while (!theKeys.empty())
					{
						theTable.insertEntry(theKeys.back());
						theKeys.pop_back();
					}
					omp_unset_lock(&lock);
				}
			}
			//Add the remaining pairs in the temporary vector in the table
			omp_set_lock(&lock);
			while (!theKeys.empty()) 
			{
				theTable.insertEntry(theKeys.back());
				theKeys.pop_back();
			}
			omp_unset_lock(&lock);
		}
		omp_destroy_lock(&lock);

		hash = "";

	}

	cout << "Rainbow Table created!" << endl;
	//write to Rainbow.txt
	writeFile("Rainbow.txt");
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
string RainbowTables::reduction(const string& hash, int column) const
{
	//implementation is specific to MD5 only
	string password;
	int index;

	unsigned char* bytes = new unsigned char[16];

	//transform to array of bytes
	hexconversion(hash.c_str(), bytes);

	for (int i = 0; i < passwordLen; i++)
	{
		index = bytes[(i + columns) % 16];
		password += chars[index % chars.size()];
	}
	delete[] bytes;
	return password;
}

string RainbowTables::createHashChain(string password) const
{
	string hash;
	//cout << "Password: " << password << ", ";

	//hash 5 times, reduce 4 times
	for (int i = 0; i < 5; i++)
	{
		if (i < 4)
		{
			hash = hashTargetWord(password);
			password = reduction(hash, i);
		}
		else
		{
			hash = hashTargetWord(password);
		}
	}
	cout << " Hash: " << hash << endl;
	//return the final result
	return hash;
}
void RainbowTables::readFile(string const& fileName)
{
	ifstream in(fileName.c_str());

	if (in)
	{
		string theHashMethod;
		in >> this->columns;
		in >> this->chars;
		in >> this->passwordLen;
		in >> theHashMethod;

		if (theHashMethod == "md5")
		{
			this->theHash = new MD5Hashing();
		}

		//read chains
		string pass, hash;
		in >> pass;
		in >> hash;
		while (in)
		{
			this->theTable.insertEntryAtEnd(hash, pass);
			in >> pass;
			in >> hash;
		}
	}
	else
	{
		cout << "Error: cannot find the file" << endl;
	}
	in.close();
}

void RainbowTables::writeFile(string const& fileName) const
{
	ofstream out(fileName.c_str());
	if (out)
	{
		theTable.print(out);
		cout << "File written successfully!" << endl;
	}
	else
	{
		cout << "Couldn't write to file" << endl;
	}
	out.close();
}
string RainbowTables::findHashInsideChain(const string& startPassword, const string& startHash) const
{
	int col = 0;
	string hash = hashTargetWord(startPassword);
	string pwd;
	// Hash and reduce the password until the end of the chain has been reached,
	// or startHash has been found.
	if (hash == startHash)
	{
		return startPassword;
	}

	//do the hash and reduce until it reaches the target hash
	while (col < this->columns)
	{
		pwd = this->reduction(hash, col);
		hash = this->hashTargetWord(pwd);
		if (hash == startHash)
		{
			return pwd;
		}
		col++;
	}
	return "";
}
string RainbowTables::getLastHash(const string& startHash, int startCol) const
{
	string hash = startHash;
	string pwd;
	// Hash and reduce the starting password <columns-starCol> times.
	for (int i = startCol; i < columns - 1; i++)
	{
		pwd = reduction(hash, i);
		hash = hashTargetWord(pwd);
	}
	return hash;
}
string RainbowTables::hashTargetWord(const string& pwd) const
{
	return theHash->hash(pwd);
}

string RainbowTables::checkPassword(const string& theHash) const
{
	string result;
#pragma omp parallel // Parallelizes the cracking. Every thread will
	// try cracking for < columns / nb of threads > different columns.
	{
		int nProc = omp_get_thread_num(); // Get thread number
		int totalProc = omp_get_num_threads(); // Get total number of threads

		string hash, pwd;
		vector<string> possiblePwd;
		vector<string>::const_iterator it;

		for (int col = columns - 1 - nProc; col >= 0 && result == ""; col -= totalProc)
		{
			// Compute the final hash, when starting at column <col>.
			hash = getLastHash(theHash, col);

			// Find the start passwords corresponding to the hash (possibly 0, 1 or more).
			possiblePwd = theTable.find(hash);

			for (it = possiblePwd.begin(); it < possiblePwd.end(); it++)
			{
				// For every start password, try to find if the hash is contained in it.
				pwd = findHashInsideChain(*it, theHash);
				cout << "pwd: " << pwd << endl;
				if (pwd != "")
				{
					// If the hash has been found, store the corresponding
					// password, causing the loop to stop.
					result = pwd;
				}
			}
		}
	}
	return result;
}
string RainbowTables::testHashReduce(const string& testWord) const
{
	string hash = hashTargetWord(testWord);
	return checkPassword(hash);
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