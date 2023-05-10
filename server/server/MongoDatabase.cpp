#include "MongoDatabase.h"
#include <bsoncxx/builder/basic/kvp.hpp>

#include "sendMsgToClientHandler.h"

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
	auto lastActiveTime = std::chrono::system_clock::time_point(std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()));

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
		<< "lastActive" << bsoncxx::types::b_date{ lastActiveTime }
		<< "active" << bsoncxx::types::b_bool{ true }
	<< finalize);
	mongoMtx.unlock();
}

bool updateUsersInfo(std::string userName, int port, int clientsPublicKey, int clientsN, int token)
{
	auto now = std::chrono::system_clock::now();
	auto lastActiveTime = std::chrono::system_clock::time_point(std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()));

	mongoMtx.lock();
	mongocxx::collection usersColl = db["Users"];

	auto filter = bsoncxx::builder::stream::document{} << "Username" << userName << bsoncxx::builder::stream::finalize;
	auto update = bsoncxx::builder::stream::document{} << "$set"
		<< bsoncxx::builder::stream::open_document
		<< "port" << port
		<< "n" << clientsN
		<< "Token" << token
		<< "publicKey" << clientsPublicKey
		<< "lastActive" << bsoncxx::types::b_date{ lastActiveTime }
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
	mongoMtx.lock();
	mongocxx::collection usersColl = db["Messages"];
	usersColl.insert_one(document{}
		<< "sendingUser" << sendingUser
		<< "receivingUser" << receivingUser
		<< "msg" << msg
		<< "msgid" << msgid
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

int getClientPort(std::string username)
{
	mongoMtx.lock();
	mongocxx::collection usersColl = db["Users"];
	bsoncxx::stdx::optional<bsoncxx::document::value> maybeResult = usersColl.find_one(document{} << "Username" << username << finalize);
	mongoMtx.unlock();

	if (maybeResult) {
		bsoncxx::document::view result = maybeResult->view();
		bsoncxx::document::element portElement = result["port"];
		if (portElement) {
			return static_cast<int>(portElement.get_int32());
		}
	}

	// Return a default port number or an error code to indicate that the client was not found.
	return -1;
}

int getClientN(std::string username)
{
	mongoMtx.lock();
	mongocxx::collection usersColl = db["Users"];
	bsoncxx::stdx::optional<bsoncxx::document::value> maybeResult = usersColl.find_one(document{} << "Username" << username << finalize);
	mongoMtx.unlock();

	if (maybeResult) {
		bsoncxx::document::view result = maybeResult->view();
		bsoncxx::document::element clientsNElement = result["n"];
		if (clientsNElement) {
			return static_cast<int>(clientsNElement.get_int32());
		}
	}

	// Return a default clientsN number or an error code to indicate that the client was not found.
	return -1;
}

int getClientPublicKey(std::string username)
{
	mongoMtx.lock();
	mongocxx::collection usersColl = db["Users"];
	bsoncxx::stdx::optional<bsoncxx::document::value> maybeResult = usersColl.find_one(document{} << "Username" << username << finalize);
	mongoMtx.unlock();

	if (maybeResult) {
		bsoncxx::document::view result = maybeResult->view();
		bsoncxx::document::element clientsPublicKeyElement = result["publicKey"];
		if (clientsPublicKeyElement) {
			return static_cast<int>(clientsPublicKeyElement.get_int32());
		}
	}

	// Return a default clientsPublicKey value or an error code to indicate that the client was not found.
	return -1;
}

void blockUser(std::string user, std::string blockedUser)
{
	mongoMtx.lock();
	mongocxx::collection usersColl = db["Users"];
	bsoncxx::stdx::optional<bsoncxx::document::value> result = usersColl.find_one(document{} << "Username" << user << finalize);

	if (result) {
		bsoncxx::document::view userDocView = result->view();
		std::string blockedList = userDocView["Blocked"].get_utf8().value.to_string();

		if (blockedList.find("," + blockedUser + ",") == std::string::npos) {
			blockedList = "," + blockedUser + "," + blockedList;
			usersColl.update_one(document{} << "Username" << user << finalize,
				document{} << "$set" << open_document
				<< "Blocked" << blockedList
				<< close_document << finalize);
		}
	}

	mongoMtx.unlock();
}

bool isBlocked(std::string user, std::string blockedUser)
{
	mongoMtx.lock();
	mongocxx::collection usersColl = db["Users"];
	bsoncxx::stdx::optional<bsoncxx::document::value> result = usersColl.find_one(document{} << "Username" << user << finalize);

	bool isUserBlocked = false;
	if (result) {
		bsoncxx::document::view userDocView = result->view();
		std::string blockedList = std::string(userDocView["Blocked"].get_utf8().value);

		isUserBlocked = (blockedList.find("," + blockedUser + ",") != std::string::npos);
	}

	mongoMtx.unlock();
	return isUserBlocked;
}

void unblockUser(std::string user, std::string unblockedUser)
{
	mongoMtx.lock();
	mongocxx::collection usersColl = db["Users"];
	bsoncxx::stdx::optional<bsoncxx::document::value> result = usersColl.find_one(document{} << "Username" << user << finalize);

	if (result) {
		bsoncxx::document::view userDocView = result->view();
		std::string blockedList = userDocView["Blocked"].get_utf8().value.to_string();

		std::string updatedBlockedList;
		size_t pos = 0;
		while (pos < blockedList.length()) {
			size_t commaPos = blockedList.find(",", pos);
			std::string curBlockedUser = blockedList.substr(pos, commaPos - pos);
			pos = commaPos + 1;

			if (curBlockedUser != unblockedUser) {
				updatedBlockedList += "," + curBlockedUser;
			}
		}

		if (!updatedBlockedList.empty()) {
			updatedBlockedList = updatedBlockedList.substr(1) + ",";
		}

		usersColl.update_one(document{} << "Username" << user << finalize,
			document{} << "$set" << open_document
			<< "Blocked" << updatedBlockedList
			<< close_document << finalize);
	}

	mongoMtx.unlock();
}

void sendMsgsFromDbToUser(std::string receivingUser, int port, int clientsPublicKey, int clientsN)
{
	json ans;
	std::string ansAsStr = " ";

	mongoMtx.lock();
	mongocxx::collection messagesColl = db["Messages"];

	bsoncxx::stdx::string_view receivingUserSV = receivingUser;
	auto filter = document{} << "receivingUser" << receivingUserSV << finalize;

	mongocxx::options::find opts{};
	opts.projection(document{} << "sendingUser" << 1 << "msg" << 1 << "msgid" << 1 << finalize);

	auto cursor = messagesColl.find(filter.view(), opts);

	for (auto&& doc : cursor) {
		std::string sendingUser = doc["sendingUser"].get_utf8().value.to_string();
		std::string msg = doc["msg"].get_utf8().value.to_string();
		int msgid = doc["msgid"].get_int32().value;

		ans["sendingUser"] = sendingUser;
		ans["msg"] = msg;
		ans["msgid"] = msgid;
		ansAsStr = ans.dump();
		std::cout << ansAsStr << " \n";
		sendMsgToClient(ansAsStr, port, clientsPublicKey, clientsN, MSG_FROM_CLIENT);
	}

	mongoMtx.unlock();
}