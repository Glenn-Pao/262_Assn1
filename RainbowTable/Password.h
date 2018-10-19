#pragma once
#ifndef PASSWORD_H
#define PASSWORD_H
#include <string>

class Password
{
private:
	bool isUsed;
	std::string thePassword;
public:
	Password();
	Password(std::string thePassword);
	~Password();

	void setUsedState(bool isUsed);

	bool getUsedState();
	std::string getPassword();
};
#endif


