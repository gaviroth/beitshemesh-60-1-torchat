#include "mongoInclude.h"
#include "MongoDatabase.h"

#include <iostream>
#include "tokenHandler.h" 
#include "blockingHandler.h"
#include "sendMsgToClientHandler.h"

void block(buffer bf)
{
	struct LoginRequest
	{
		std::string token;
		std::string username;
		std::string blockedUser;
	};

	int port = 0;
	int clientsN = 0;
	int clientsPublicKey = 0;

	json ans;
	std::string ansAsStr = " ";

	std::string dataValue(bf.begin(), bf.end());
	json j = json::parse(dataValue);

	LoginRequest finalData = { j["token"], j["username"], j["blockedUser"] };

	if (doesUserExist(finalData.username))
	{
		if (doesUserExist(finalData.blockedUser))
		{
			if (isTokenValid(finalData.username, std::stoi(finalData.token)))
			{
				blockUser(finalData.username, finalData.blockedUser);
				ans["msg"] = "user blocked successfully";
				ansAsStr = ans.dump();

				int port = getClientPort(finalData.username);
				int clientsN = getClientN(finalData.username);
				int clientsPublicKey = getClientPublicKey(finalData.username);

				sendMsgToClient(ansAsStr, port, clientsPublicKey, clientsN, USER_BLOCKED_SUCCESSFULLY);
				std::cout << "user blocked successfully \n";
			}
			else {
				ans["msg"] = "token isnt valid please log in";
				ansAsStr = ans.dump();

				int port = getClientPort(finalData.username);
				int clientsN = getClientN(finalData.username);
				int clientsPublicKey = getClientPublicKey(finalData.username);

				sendMsgToClient(ansAsStr, port, clientsPublicKey, clientsN, TOKEN_ISNT_VALID);
				std::cout << "token isnt valid please log in \n";
			}
		}
		else {
			ans["msg"] = "user you want to block dosnt exists";
			ansAsStr = ans.dump();

			int port = getClientPort(finalData.username);
			int clientsN = getClientN(finalData.username);
			int clientsPublicKey = getClientPublicKey(finalData.username);

			sendMsgToClient(ansAsStr, port, clientsPublicKey, clientsN, USER_DOSNT_EXIST);
			std::cout << "user you want to block dosnt exists \n";
		}

	}
	else {
		std::cout << "user dosnt exists \n";
	}
}

void unblock(buffer bf)
{
	struct LoginRequest
	{
		std::string token;
		std::string username;
		std::string unblockedUser;
	};

	int port = 0;
	int clientsN = 0;
	int clientsPublicKey = 0;

	json ans;
	std::string ansAsStr = " ";

	std::string dataValue(bf.begin(), bf.end());
	json j = json::parse(dataValue);

	LoginRequest finalData = { j["token"], j["username"], j["unblockedUser"] };

	if (doesUserExist(finalData.username))
	{
		if (doesUserExist(finalData.unblockedUser))
		{
			if (isTokenValid(finalData.username, std::stoi(finalData.token)))
			{
				unblockUser(finalData.username, finalData.unblockedUser);
				ans["msg"] = "user unblocked successfully";
				ansAsStr = ans.dump();

				int port = getClientPort(finalData.username);
				int clientsN = getClientN(finalData.username);
				int clientsPublicKey = getClientPublicKey(finalData.username);

				sendMsgToClient(ansAsStr, port, clientsPublicKey, clientsN, USER_UNBLOCKED_SUCCESSFULLY);
				std::cout << "user unblocked successfully \n";
			}
			else {
				ans["msg"] = "token isnt valid please log in";
				ansAsStr = ans.dump();

				int port = getClientPort(finalData.username);
				int clientsN = getClientN(finalData.username);
				int clientsPublicKey = getClientPublicKey(finalData.username);

				sendMsgToClient(ansAsStr, port, clientsPublicKey, clientsN, TOKEN_ISNT_VALID);
				std::cout << "token isnt valid please log in \n";
			}
		}
		else {
			ans["msg"] = "user you want to unblock dosnt exists";
			ansAsStr = ans.dump();

			int port = getClientPort(finalData.username);
			int clientsN = getClientN(finalData.username);
			int clientsPublicKey = getClientPublicKey(finalData.username);

			sendMsgToClient(ansAsStr, port, clientsPublicKey, clientsN, USER_DOSNT_EXIST);
			std::cout << "user you want to unblock dosnt exists \n";
		}
	}
	else {
		std::cout << "user dosnt exists \n";
	}
}