#include "mongoInclude.h"
#include "MongoDatabase.h"

#include <iostream>
#include "msgHandler.h"
#include "tokenHandler.h" 
#include "sendMsgToClientHandler.h"

void handelMsg(buffer bf, int port, int clientsPublicKey, int clientsN)
{
	struct msgFields
	{
		std::string msg;
		std::string token;
		std::string sendingUser;
		std::string receivingUser;
	};

	int token = 0;
	std::string Stoken = " ";

	std::string dataValue(bf.begin(), bf.end());
	json j = json::parse(dataValue);

	msgFields finalData = { j["msg"], j["token"], j["sendingUser"], j["receivingUser"] };
	/*
	if (doesUserExist(finalData.receivingUser))//check if receiving user exists 
	{
		
		if (doesUserExist(finalData.sendingUser))//check if sending User exists 
		{
			if (isTokenValid)
			{

			}
			else {

			}
		}
		else {

		}
	}
	else {

	}
	*/
}