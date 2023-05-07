#include "mongoInclude.h"
#include "MongoDatabase.h"

#include <iostream>
#include "tokenHandler.h" 
#include "SignUpRequestHandler.h"
#include "sendMsgToClientHandler.h"

void signup(buffer bf, int port, int clientsPublicKey , int clientsN)
{
	struct SignupRequest
	{
		std::string email;
		std::string username;
		std::string password;
	};

	json ans;
	std::string ansAsStr = " ";

	int token = 0;
	std::string tokenAsStr = " ";

	std::string dataValue(bf.begin(), bf.end());
	//std::cout << dataValue;
	dataValue = dataValue.substr(1);
	json j = json::parse(dataValue);

	SignupRequest finalData = { j["Email"], j["Username"], j["Password"] };

	std::regex emailCheck("^([a-zA-Z0-9._-]+@[a-zA-Z0-9._-]+\.[a-zA-Z0-9_-]+)$");//name and @ and then name . name
	std::regex passwordCheck("^(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[@$!%*^&])[a-zA-Z0-9@$!%*^&]{8,8}$");//8 digits one uper case one lower case one number and a symbel
	
	if (!doesUserExist(finalData.username))//check if username is taken
	{
		//check that password and email are valid 
		if (std::regex_match(finalData.password, passwordCheck))
		{
			if (std::regex_match(finalData.email, emailCheck))
			{
				token = generateToken();
				tokenAsStr = std::to_string(token);
				addNewUser(finalData.username, finalData.password, finalData.email, port, clientsPublicKey, clientsN, token); // sign up to the db
				
				ans["msg"] = "user signed up successfully";
				ans["token"] = tokenAsStr;
				ansAsStr = ans.dump();

				sendMsgToClient(ansAsStr, port, clientsPublicKey, clientsN, CLIENT_SIGNED_UP_SUCCESSFULLY);
				std::cout << "user signed up successfully \n";
			}
			else {
				ans["msg"] = "email is not valid";
				ansAsStr = ans.dump();

				sendMsgToClient(ansAsStr, port , clientsPublicKey, clientsN, EMAIL_NOT_VALID);
				std::cout << "email is not valid \n";
			}
		}   
		else{
			ans["msg"] = "password is not valid";
			ansAsStr = ans.dump();
			
			sendMsgToClient(ansAsStr, port, clientsPublicKey, clientsN, PASSWORD_NOT_VALID);
			std::cout << "password is not valid \n";
		}
	}
	else {
		ans["msg"] = "username already taken";
		ansAsStr = ans.dump();
		
		sendMsgToClient(ansAsStr, port, clientsPublicKey, clientsN, USERNAME_ALREADY_TAKEN);
		std::cout << "username already taken \n";
	}
}