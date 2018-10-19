#pragma once
#ifndef HASHCHAIN_H
#define HASHCHAIN_H
#include <string>
class HashChain
{
private:
	std::string chainStartPassword;
	std::string chainLastHash;
public:
	HashChain();
	HashChain(std::string chainStartPassword, std::string chainLastHash);
	~HashChain();

	std::string getChainStartPassword();
	std::string getChainLastHash();
};
#endif
