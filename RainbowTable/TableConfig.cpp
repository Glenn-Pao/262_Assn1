#include "TableConfig.h"

TableConfig::TableConfig()
{
}


TableConfig::~TableConfig()
{
}

int TableConfig::findIndex(const string& hash) const
{
	// get the min, mid and max values
	int min = 0, max = theKeys.size() - 1;
	int mid = (min + max) / 2;
	int comp;	//the comparison difference between target and the table
	
	while (min <= max) 
	{
		//check the difference in string length
		comp = hash.compare(theKeys[mid].getHash());

		//equal
		if (comp == 0) 
		{
			return mid;
		}
		//there is a mismatch, string is shorter
		else if (comp < 0) 
		{
			max = mid - 1;
		}
		//there is a mismatch, string is longer
		else 
		{
			min = mid + 1;
		}
		mid = (min + max) / 2;
	}
	return min;
}

//find a hash in the table and return the corresponding password
vector<string> TableConfig::find(const string& hash) const
{
	//search using binary search algorithm
	//returns all of the corresponding passwords

	vector<string> results;
	// get the min, mid and max values
	int min = 0, max = theKeys.size() - 1;
	int mid = (min + max) / 2;
	int comp;	//the comparison difference between target and the table

	while (min <= max)
	{
		//check the difference in string length
		comp = hash.compare(theKeys[mid].getHash());
		cout << "Hash Target: " << hash << endl;
		cout << "This hash  : " << theKeys[mid].getHash() << endl << endl;

		//equal
		if (comp == 0)
		{
			//insert the password
			results.insert(results.begin(), theKeys[mid].getPwd());
			// Find all the other elements with key hash before mid, add them.
			min = mid - 1;
			while (min >= 0 && hash.compare(theKeys[min].getHash()) == 0) 
			{
				results.insert(results.begin(), theKeys[min--].getPwd());
			}
			// Find all the other elements with key hash after mid, add them.
			min = mid + 1;
			while (min < theKeys.size() && hash.compare(theKeys[min].getHash()) == 0) 
			{
				results.insert(results.begin(), theKeys[min++].getPwd());
			}
			return results;
		}
		//there is a mismatch, string is shorter
		else if (comp < 0)
		{
			max = mid - 1;
		}
		//there is a mismatch, string is longer
		else
		{
			min = mid + 1;
		}
		mid = (min + max) / 2;
	}
	return results;
}

//add a new hash entry
void TableConfig::insertEntry(const Keys& key)
{
	int index = findIndex(key.getHash());
	theKeys.push_back(key);
}

//alternate method to inser pair at end of table. Used when loading from file to reduce loading time
void TableConfig::insertEntryAtEnd(const string& hash, const string& pwd)
{
	Keys theKey(pwd, hash);
	theKeys.push_back(theKey);
}

//print the table content
ostream& TableConfig::print(ostream& stream) const
{
	vector<Keys>::const_iterator it;
	for (it = theKeys.begin(); it < theKeys.end(); it++)
	{
		stream << (*it).getPwd() << ":" << (*it).getHash() << endl;
	}
	return stream;
}

void TableConfig::freeMem()
{
	theKeys.clear();
}