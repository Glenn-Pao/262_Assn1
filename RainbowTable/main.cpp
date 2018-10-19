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
	RainbowTables mainTable = RainbowTables("assn1words.txt", "Rainbow.txt");
}

//request for hash
void Part3()
{
	RainbowTables mainTable = RainbowTables("assn1words.txt", "Rainbow.txt");
	mainTable.main();
}

int main()
{
	Part3();
	return 0;
}