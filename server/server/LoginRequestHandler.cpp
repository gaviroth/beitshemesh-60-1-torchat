#include "mongoInclude.h"
#include "MongoDatabase.h"

#include <iostream>
#include "tokenHandler.h" 
#include "LoginRequestHandler.h"
#include "sendMsgToClientHandler.h"

void login(buffer bf, int port, int clientsPublicKey, int clientsN)
{
	struct LoginRequest
	{
		std::string username;
		std::string password;
	};

	json ans;
	std::string ansAsStr = " ";

	int token = 0; 
	std::string tokenAsStr = " ";

	std::string dataValue(bf.begin(), bf.end());
	dataValue = dataValue.substr(1);
	json j = json::parse(dataValue);

	LoginRequest finalData = { j["username"], j["password"] };

	if (doesUserExist(finalData.username))
	{
		if (doesPasswordMatch(finalData.username, finalData.password)) 
		{

			token = generateToken();
			tokenAsStr = std::to_string(token);
			updateUsersInfo(finalData.username, port, clientsPublicKey, clientsN, token);

			ans["msg"] = "user loged in successfully";
			ans["token"] = tokenAsStr;
			ansAsStr = ans.dump();

			sendMsgToClient(ansAsStr, port, clientsPublicKey, clientsN, CLIENT_LOGED_IN_SUCCESSFULLY);
			std::cout << "user loged in successfully \n";

			sendMsgsFromDbToUser(finalData.username, port, clientsPublicKey, clientsN);
		}
		else {
			ans["msg"] = "wrong password";
			ansAsStr = ans.dump();

			sendMsgToClient(ansAsStr, port, clientsPublicKey, clientsN, PASSWORD_DOSNT_MATCH);
			std::cout << "password dosnt match \n";
		}
	}
	else {
		ans["msg"] = "user dosnt exist Please sign up";
		ansAsStr = ans.dump();

		sendMsgToClient(ansAsStr, port, clientsPublicKey, clientsN, USER_DOSNT_EXIST);
		std::cout << "user dosnt exists \n";
	}
}