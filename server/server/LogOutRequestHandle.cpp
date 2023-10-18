#include "mongoInclude.h"
#include "MongoDatabase.h"

#include <iostream>
#include "LogOutRequestHandler.h"

void logOut(buffer bf)
{
	struct LoginRequest
	{
		std::string username;
	};
	std::string dataValue(bf.begin(), bf.end());
	dataValue = dataValue.substr(1);

	json j = json::parse(dataValue);

	LoginRequest finalData = { j["username"]};

	if (doesUserExist(finalData.username))
	{
		updateStatusToOffline(finalData.username);
		std::cout << "user loged out successfully \n";
	}
	else {
		std::cout << "user dosnt exists \n";
	}
}