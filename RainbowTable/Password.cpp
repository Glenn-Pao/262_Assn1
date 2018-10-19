#include "Password.h"

Password::Password()
{
	isUsed = false;
	thePassword = "";
}
Password::Password(std::string thePassword)
{
	isUsed = false;
	this->thePassword = thePassword;
}
Password::~Password()
{
}
void Password::setUsedState(bool isUsed)
{
	this->isUsed = isUsed;
}
bool Password::getUsedState()
{
	return isUsed;
}
std::string Password::getPassword()
{
	return thePassword;
}