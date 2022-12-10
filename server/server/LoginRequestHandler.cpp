#include "LoginRequestHandler.h"
#include "mongoInclude.h"
#include "MongoDatabase.h"
#include <iostream>

void login(buffer bf, buffer port) {
	struct LoginRequest
	{
		std::string username;
		std::string password;
	};

	std::string dataValue(bf.begin(), bf.end());

	json j = json::parse(dataValue);

	LoginRequest finalData = { j["username"], j["password"] };

	if (doesUserExist(finalData.username))
	{
		if (doesPasswordMatch(finalData.username, finalData.password)) 
		{
			std::cout << "user loged in successfully \n";
		}
		else {
			std::cout << "password dosnt match \n";
		}
	}
	else {
		std::cout << "user dosnt exists \n";
	}
}