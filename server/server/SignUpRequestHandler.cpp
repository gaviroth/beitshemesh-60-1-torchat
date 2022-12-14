#include "mongoInclude.h"
#include "MongoDatabase.h"

#include <iostream>
#include "SignUpRequestHandler.h"
#include "sendMsgToClientHandler.h"

void signup(buffer bf, int port) {
	struct SignupRequest
	{
		std::string email;
		std::string username;
		std::string password;
	};


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
				addNewUser(finalData.username, finalData.password, finalData.email); // sign up to the db
				std::cout << "user signed up successfully \n";
				sendMsgToClient("user signed up successfully", port);
			}
			else {
				//std::cout << "email is not valid \n";
				sendMsgToClient("email is not valid", port);
			}
		}   
		else{

			//std::cout << "password is not valid \n";
			sendMsgToClient("password is not valid", port);
		}
	}
	else {

		//std::cout << "Username already taken \n";
		sendMsgToClient("Username already taken", port);
	}
}