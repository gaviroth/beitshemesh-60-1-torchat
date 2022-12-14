#include "mongoInclude.h"
#include "MongoDatabase.h"

#include <iostream>
#include "LoginRequestHandler.h"
#include "sendMsgToClientHandler.h"

void login(buffer bf, int port) {
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
			//std::cout << "user loged in successfully \n";
			sendMsgToClient("user loged in successfully", port);
		}
		else {
			//std::cout << "password dosnt match \n";
			sendMsgToClient("password dosnt match", port);
		}
	}
	else {
		//std::cout << "user dosnt exists \n";
		sendMsgToClient("user dosnt exists", port);
	}
}