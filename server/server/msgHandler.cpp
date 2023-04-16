#include "mongoInclude.h"
#include "MongoDatabase.h"

#include <ctime>
#include <cstdlib>
#include <iostream>
#include "msgHandler.h"
#include "tokenHandler.h" 
#include "sendMsgToClientHandler.h"

extern std::mutex idMtx;
extern std::set<int> idSet;

void handelMsg(buffer bf, int port, int clientsPublicKey, int clientsN)
{
	int id = 0;
	struct msgFields
	{
		std::string msg;
		std::string token;
		std::string sendingUser;
		std::string receivingUser;
	};

	std::string dataValue(bf.begin(), bf.end());
	json j = json::parse(dataValue);

	msgFields finalData = { j["msg"], j["token"], j["sendingUser"], j["receivingUser"] };

	if (doesUserExist(finalData.receivingUser))//check if receiving user exists 
	{
		
		if (doesUserExist(finalData.sendingUser))//check if sending User exists 
		{
			if (isTokenValid)
			{ 
				id = generateid();
				addNewmsg(finalData.sendingUser, finalData.receivingUser, finalData.msg, id);
				//send msg to receiving user
				if (isUserActive(finalData.receivingUser)) {
					sendMsgToClient(finalData.msg, port, clientsPublicKey, clientsN, MSG_FROM_CLIENT);
				}
			}
			else {
				sendMsgToClient("token isnt valid please log in again" , port, clientsPublicKey, clientsN, CLIENT_LOG_IN_RESPONSE);
			}
		}
		else {
			sendMsgToClient("user sendin msg dosnt exists" , port, clientsPublicKey, clientsN, CLIENT_LOG_IN_RESPONSE);
		}
	}
	else {
		sendMsgToClient("user msg is for dosnt exists" , port, clientsPublicKey, clientsN, CLIENT_LOG_IN_RESPONSE);
	}

}

std::string returnJsonAsString() 
{
	return "a";
}

// function returns a random 6 digit token 
int randomid()
{
	std::srand(std::time(nullptr));

	int num1 = 0;
	int num2 = 0;
	int num3 = 0;
	int num4 = 0;
	int num5 = 0;
	int num6 = 0;
	int num7 = 0;
	int id = 0;

	num1 = (rand() % 9) + 1;
	num2 = (rand() % 9) + 1;
	num3 = (rand() % 9) + 1;
	num4 = (rand() % 9) + 1;
	num5 = (rand() % 9) + 1;
	num6 = (rand() % 9) + 1;
	num7 = (rand() % 9) + 1;

	id = num1 * 1000000;
	id += num2 * 100000;
	id += num3 * 10000;
	id += num4 * 1000;
	id += num5 * 100;
	id += num6 * 10;
	id += num7;

	return id;
}

/*Function generates a token, makes sure it's
not taken by another user and saves it to Mongo dB
(because the next request will be to a different thread)
and saves to the set so you can make sure token isn't taken*/
int generateid()
{
	int id = 0;
	std::set<int>::iterator it;

	idMtx.lock();
	do {
		id = randomid();
		it = idSet.find(id);
	} while (it != idSet.end());

	idSet.insert(id);
	idMtx.unlock();

	return id;
}

void eraseidFromSet(int id)
{
	idMtx.lock();
	idSet.erase(id);
	idMtx.unlock();
}