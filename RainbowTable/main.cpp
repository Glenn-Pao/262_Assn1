#include <iostream>
#include <string>
#include <fstream>
#include "RainbowTables.h"
using namespace std;

//read the list of possible passwords
int Part1()
{
	//initialize counter and a string buffer
	int count = 0;
	string line;

	cout << "Counting number of possible passwords" << endl;

	//load up the target file
	ifstream file("assn1words.txt");

	//increment the count with every line is detected
	while (getline(file, line))
	{
		//cout << line << endl;
		count++;
	}

	//report the total count. the expected result is 25143
	cout << "Number of possible passwords in this file: " << count << endl;

	//use the count to create the number of rows
	return count;
}

//establish the rainbow table by hashing and reducing
void Part2()
{
	//original = 1
	// hash = 5
	// reduce = 4
	//altogether = 10
	RainbowTables mainTable = RainbowTables(2, Part1(), "assn1words.txt");
}

//request for hash
void Part3()
{
	bool running = true;
#pragma region Setting up rainbow table
	//original = 1
	// hash = 5
	// reduce = 4
	//altogether = 10
	RainbowTables mainTable = RainbowTables(2, Part1(), "assn1words.txt");
#pragma endregion
	
	while (running)
	{
		int choice = 0;
		string targetHash;
		bool foundHash = false;

		cout << "===============" << endl;
		cout << " Find Password " << endl;
		cout << "===============" << endl;

		cout << "1) Input hash value" << endl;
		cout << "2) Quit" << endl;
		cout << "Choose an action: ";
		cin >> choice;

		switch (choice)
		{
		case 1:
		{
			cout << "Define the hash value." << endl;
			cin >> targetHash;

			string resolve = mainTable.testHashReduce(targetHash);

			cout << "Hash: " << targetHash << " -->";

			//check for the condition where resolve is empty
			foundHash = (resolve == "");

			if (foundHash)
			{
				cout << "Couldn't find the hash.." << endl;
			}
			else
			{
				cout << "Cracked Password: " << resolve << endl;
			}
		}
			break;
		case 2:
			cout << "Thank you for using this program" << endl;
			running = false;
			break;
		default:
			cout << "Invalid input detected. Please try again." << endl;
			break;
		}

		
	}
}

int main()
{
	Part3();
	return 0;
}