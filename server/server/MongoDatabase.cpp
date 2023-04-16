#include "MongoDatabase.h"
#include <bsoncxx/builder/basic/kvp.hpp>

extern std::mutex mongoMtx;
extern mongocxx::client cli;
extern mongocxx::database db;
extern mongocxx::uri uri;

void creat()
{ 
	cli = mongocxx::client(uri);
	db = cli["TorChat"];
}

void addNewUser(std::string username, std::string password, std::string email, int port, int clientsPublicKey, int clientsN, int token)
{
	auto now = std::chrono::system_clock::now();
	auto now_c = std::chrono::system_clock::to_time_t(now);

	mongoMtx.lock();
	mongocxx::collection usersColl = db["Users"];
	usersColl.insert_one(document{}
		<< "Username" << username
		<< "Password" << password
		<< "Email" << email
		<< "Blocked" << ""
		<< "port" << port
		<< "n" << clientsN
		<< "Token" << token
		<< "publicKey" << clientsPublicKey
		<< "lastActive" << std::ctime(&now_c)
		<< "active" << bsoncxx::types::b_bool{ true }
	<< finalize);
	mongoMtx.unlock();
}

bool updateUsersInfo(std::string userName, int port, int clientsPublicKey, int clientsN, int token)
{
	auto now = std::chrono::system_clock::now();
	auto now_c = std::chrono::system_clock::to_time_t(now);

	mongoMtx.lock();
	mongocxx::collection usersColl = db["Users"];

	auto filter = bsoncxx::builder::stream::document{} << "Username" << userName << bsoncxx::builder::stream::finalize;
	auto update = bsoncxx::builder::stream::document{} << "$set"
		<< bsoncxx::builder::stream::open_document
		<< "port" << port
		<< "n" << clientsN
		<< "Token" << token
		<< "publicKey" << clientsPublicKey
		<< "lastActive" << std::ctime(&now_c)
		<< "active" << bsoncxx::types::b_bool{ true }
		<< bsoncxx::builder::stream::close_document
		<< bsoncxx::builder::stream::finalize;

	auto update_result = usersColl.update_one(filter.view(), update.view());
	mongoMtx.unlock();

	if (update_result.has_value() == true) {
		return true;
	}

	return false;
}

bool doesUserExist(std::string username)
{
	mongoMtx.lock();
	mongocxx::collection usersColl = db["Users"];
	mongocxx::stdx::optional<bsoncxx::document::value> check = usersColl.find_one(document{} << "Username" << username << finalize);

	mongoMtx.unlock();
	if (check.has_value() == true)
	{
		return true;
	}

	return false;
}

bool doesPasswordMatch(std::string username, std::string password)
{
	mongoMtx.lock();
	mongocxx::collection usersColl = db["Users"];
	mongocxx::stdx::optional<bsoncxx::document::value> check = usersColl.find_one(document{} << "Username" << username <<
		"Password" << password << finalize);

	mongoMtx.unlock();
	if (check.has_value() == true)
	{
		return true;
	}

	return false;
}


bool updateToken(int token, std::string username)
{
	mongoMtx.lock();
	mongocxx::collection usersColl = db["Users"];

	auto filter = bsoncxx::builder::stream::document{} << "Username" << username << bsoncxx::builder::stream::finalize;
	auto update = bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << "Token" << token << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

	auto update_result = usersColl.update_one(filter.view(), update.view());
	mongoMtx.unlock();

	if (update_result.has_value() == true)
	{
		return true;
	}

	return false;
}

bool updateStatusToOffline(std::string username)
{
	mongoMtx.lock();
	mongocxx::collection usersColl = db["Users"];

	auto filter = bsoncxx::builder::stream::document{} << "Username" << username << bsoncxx::builder::stream::finalize;
	auto update = bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << "active" << bsoncxx::types::b_bool{ false } << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

	auto update_result = usersColl.update_one(filter.view(), update.view());
	mongoMtx.unlock();

	if (update_result.has_value() == true)
	{
		return true;
	}

	return false;
}

bool doesTokenMatch(int Token, std::string username)
{
	mongoMtx.lock();
	mongocxx::collection usersColl = db["Users"];

	auto filter = bsoncxx::builder::stream::document{} << "Username" << username << bsoncxx::builder::stream::finalize;

	auto result = usersColl.find_one(filter.view());
	auto view = result->view();

	int storedToken = view["Token"].get_int32();
	mongoMtx.unlock();

	if (Token == storedToken) {
		return true;
	}

	return false;
}

bool isUserActive(std::string username)
{
	mongoMtx.lock();
	mongocxx::collection usersColl = db["Users"];

	auto filter = bsoncxx::builder::stream::document{} << "Username" << username << bsoncxx::builder::stream::finalize;

	auto result = usersColl.find_one(filter.view());
	auto view = result->view();

	bool isActive = false;
	if (view["active"].type() == bsoncxx::type::k_bool) {
		isActive = view["active"].get_bool().value;
	}

	mongoMtx.unlock();

	return isActive;
}

bool isUserInactiveForHalfHour(std::string userName)
{
	mongoMtx.lock();
	mongocxx::collection usersColl = db["Users"];

	auto filter = bsoncxx::builder::stream::document{} << "Username" << userName << bsoncxx::builder::stream::finalize;

	auto result = usersColl.find_one(filter.view());
	auto view = result->view();

	auto lastActiveTime = view["lastActive"].get_date();
	auto lastActiveTimePoint = std::chrono::system_clock::from_time_t(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::milliseconds(lastActiveTime)).count());

	auto now = std::chrono::system_clock::now();
	auto now_c = std::chrono::system_clock::to_time_t(now);

	std::chrono::duration<double> halfHour(1800); // 1800 seconds = 30 minutes
	bool isInactive = (now - lastActiveTimePoint) > halfHour;

	mongoMtx.unlock();

	return isInactive;
}

void addNewmsg(std::string sendingUser, std::string receivingUser, std::string msg, int msgid)
{
	auto now = std::chrono::system_clock::now();
	auto now_c = std::chrono::system_clock::to_time_t(now);

	mongoMtx.lock();
	mongocxx::collection usersColl = db["Messages"];
	usersColl.insert_one(document{}
		<< "sendingUser" << sendingUser
		<< "receivingUser" << receivingUser
		<< "msg" << msg
		<< "msgid" << msgid
		<< "time" << std::ctime(&now_c)
	<< finalize);
	mongoMtx.unlock();
}

void deleteMsgById(int msgid)
{
	mongoMtx.lock();
	mongocxx::collection usersColl = db["Messages"];
	usersColl.delete_one(document{} << "msgid" << msgid << finalize);
	mongoMtx.unlock();
}