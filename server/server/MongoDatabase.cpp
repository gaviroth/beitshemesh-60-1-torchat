#include "MongoDatabase.h"

#include <bsoncxx/builder/basic/kvp.hpp>


//mongocxx::uri uri;
mongocxx::client cli;
mongocxx::database db;
mongocxx::uri uri{ "mongodb://localhost:27017" };

void creat()
{ 
	cli = mongocxx::client(uri);
	db = cli["TorChat"];
}


bool doesUserExist(std::string username)
{
	mongocxx::collection usersColl = db["Users"];
	mongocxx::stdx::optional<bsoncxx::document::value> check = usersColl.find_one(document{} << "Username" << username << finalize);

	if (check.has_value() == true)
	{
		return true;
	}

	return false;
}

bool doesPasswordMatch(std::string username, std::string password)
{
	mongocxx::collection usersColl = db["Users"];
	mongocxx::stdx::optional<bsoncxx::document::value> check = usersColl.find_one(document{} << "Username" << username <<
		"Password" << password << finalize);

	if (check.has_value() == true)
	{
		return true;
	}

	return false;
}

void addNewUser(std::string username, std::string password, std::string email)
{
	mongocxx::collection usersColl = db["Users"];
	usersColl.insert_one(document{} 
		<< "Username" << username
		<< "Password" << password
		<< "Email" << email
		<< "Blocked" << ""
		<< "Token" << 0
		<< "Date" << ""
		<< "active" << ""
		<< finalize);
}

bool updateToken(int Token, std::string userName) {
	mongocxx::collection usersColl = db["Users"];
	
	auto filter = bsoncxx::builder::stream::document{} << "Username" << userName << bsoncxx::builder::stream::finalize;
	auto update = bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << "Token" << Token << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

	auto update_result = usersColl.update_one(filter.view(), update.view());
	if (update_result.has_value() == true)
	{
		return true;
	}

	return false;
}