#include "mongoInclude.h"
#include "MongoDatabase.h"

#include <iostream>
#include "tokenHandler.h" 
#include "SignUpRequestHandler.h"
#include "sendMsgToClientHandler.h"

void signup(buffer bf, int port, int clientsPublicKey , int clientsN) {
	struct SignupRequest
	{
		std::string email;
		std::string username;
		std::string password;
	};

	int token = 0;
	std::string Stoken = " ";

	std::string dataValue(bf.begin(), bf.end());
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
				addNewUser(finalData.username, finalData.password, finalData.email, port, clientsPublicKey, clientsN, token); // sign up to the db
				Stoken = std::to_string(token);
				sendMsgToClient("user signed up successfully" + Stoken, port, clientsPublicKey, clientsN, CLIENT_SIGN_UP_RESPONSE);
				std::cout << "user signed up successfully \n";
			}
			else {
				std::cout << "email is not valid \n";
				sendMsgToClient("email is not valid", port , clientsPublicKey, clientsN, CLIENT_SIGN_UP_RESPONSE);
			}
		}   
		else{

			std::cout << "password is not valid \n";
			sendMsgToClient("password is not valid", port, clientsPublicKey, clientsN, CLIENT_SIGN_UP_RESPONSE);
		}
	}
	else {

		std::cout << "username already taken \n";
		sendMsgToClient("username already taken", port, clientsPublicKey, clientsN, CLIENT_SIGN_UP_RESPONSE);
	}
}