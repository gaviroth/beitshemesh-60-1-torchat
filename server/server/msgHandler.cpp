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

void handelMsg(buffer bf)
{
	int msgid = 0;
	struct msgFields
	{
		std::string msg;
		std::string token;
		std::string sendingUser;
		std::string receivingUser;
	};

	int port = 0;
	int clientsN = 0;
	int clientsPublicKey = 0;

	json ans;
	std::string ansAsStr = " ";

	std::string dataValue(bf.begin(), bf.end());
	dataValue = dataValue.substr(1);
	json j = json::parse(dataValue);

	msgFields finalData = { j["msg"], j["token"], j["sendingUser"], j["receivingUser"] };

	int token = std::stoi(finalData.token);

	if (doesUserExist(finalData.sendingUser))// check if sending User exists 
	{
		if (doesUserExist(finalData.receivingUser))//check if receiving user exists 
		{
			if (isTokenValid(finalData.sendingUser, token))
			{
				ans["msg"] = "message sent successfully";
				ansAsStr = ans.dump();

				int port = getClientPort(finalData.sendingUser);
				int clientsN = getClientN(finalData.sendingUser);
				int clientsPublicKey = getClientPublicKey(finalData.sendingUser);

				sendMsgToClient(ansAsStr, port, clientsPublicKey, clientsN, MESSAGE_SENT_SUCCESSFULLY);
				std::cout << "message sent successfully ,sent to user\n";
				if (!isBlocked(finalData.receivingUser, finalData.sendingUser))
				{
					msgid = generateMsgid();
					addNewmsg(finalData.sendingUser, finalData.receivingUser, finalData.msg, msgid);
					//send msg to receiving user
					if (isUserActive(finalData.receivingUser)) {
						if (!isUserInactiveForHalfHour(finalData.receivingUser))
						{
							ans["sendingUser"] = finalData.sendingUser;
							ans["msg"] = finalData.msg;
							ans["id"] = msgid;
							ansAsStr = ans.dump();

							int port = getClientPort(finalData.receivingUser);
							int clientsN = getClientN(finalData.receivingUser);
							int clientsPublicKey = getClientPublicKey(finalData.receivingUser);

							sendMsgToClient(ansAsStr, port, clientsPublicKey, clientsN, MSG_FROM_CLIENT);
						}
					}
				}
			}
			else {
				ans["msg"] = "token isnt valid please log in";
				ansAsStr = ans.dump();

				int port = getClientPort(finalData.sendingUser);
				int clientsN = getClientN(finalData.sendingUser);
				int clientsPublicKey = getClientPublicKey(finalData.sendingUser);

				sendMsgToClient(ansAsStr, port, clientsPublicKey, clientsN, TOKEN_ISNT_VALID);
				std::cout << "token isnt valid please log in \n";
			}
		}
		else {
			ans["msg"] = "user message is for dosnt exists";
			ansAsStr = ans.dump();

			int port = getClientPort(finalData.sendingUser);
			int clientsN = getClientN(finalData.sendingUser);
			int clientsPublicKey = getClientPublicKey(finalData.sendingUser);

			sendMsgToClient(ansAsStr, port, clientsPublicKey, clientsN, USER_DOSNT_EXIST);
			std::cout << "user message is for dosnt exists \n";
		}
	}
	else {
		std::cout << "user dosnt exists \n";
	}

}

void msgReceived(buffer bf)//dont know if i need //dataValue = dataValue  + '}';
{
	int msgid = 0;
	struct msgFields
	{
		std::string msgid;
	};

	std::string dataValue(bf.begin(), bf.end());
	dataValue = dataValue.substr(1);
	json j = json::parse(dataValue);

	msgFields finalData = { j["msgid"]};

	deleteMsgById(std::stoi(finalData.msgid));
	eraseidFromSet(std::stoi(finalData.msgid));
}

// function returns a random 7 digit id
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

/*Function generates a id, makes sure it's
not taken by another user and saves it to Mongo dB
(because the next request will be to a different thread)
and saves to the set so you can make sure token isn't taken*/
int generateMsgid()
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

void eraseidFromSet(int msgid)
{
	idMtx.lock();
	idSet.erase(msgid);
	idMtx.unlock();
}