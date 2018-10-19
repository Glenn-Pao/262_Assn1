#include "HashChain.h"

HashChain::HashChain()
{
	chainStartPassword = "";
	chainLastHash = "";
}
HashChain::HashChain(std::string chainStartPassword, std::string chainLastHash)
{
	this->chainStartPassword = chainStartPassword;
	this->chainLastHash = chainLastHash;
}
HashChain::~HashChain()
{
}
std::string HashChain::getChainStartPassword()
{
	return chainStartPassword;
}
std::string HashChain::getChainLastHash()
{
	return chainLastHash;
}