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
	json j = json::parse(dataValue);

	LoginRequest finalData = { j["username"]};

	if (doesUserExist(finalData.username))
	{
		updateStatusToOffline(finalData.username);
	}
	else {
		std::cout << "user dosnt exists \n";
	}
}