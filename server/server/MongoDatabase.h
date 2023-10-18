#pragma once
#include "mongoInclude.h"
#include "buffer.h"
#include <chrono>
#include <mutex>
#include <ctime>
using nlohmann::json;

void creat();
void deleteMsgById(int msgid);
int getClientN(std::string username);
int getClientPort(std::string username);
bool isUserActive(std::string username);
bool doesUserExist(std::string username);
int getClientPublicKey(std::string username);
bool updateStatusToOffline(std::string username);
bool updateToken(int Token, std::string username);
bool doesTokenMatch(int Token, std::string username);
bool isUserInactiveForHalfHour(std::string userName);
bool isBlocked(std::string user, std::string blockedUser);
void blockUser(std::string user, std::string blockedUser);
void unblockUser(std::string user, std::string unblockedUser);
bool doesPasswordMatch(std::string username, std::string password);
void addNewmsg(std::string sendingUser, std::string receivingUser, std::string msg, int msgid);
void sendMsgsFromDbToUser(std::string receivingUser, int port, int clientsPublicKey, int clientsN);
bool updateUsersInfo(std::string userName, int port, int clientsPublicKey, int clientsN, int token);
void addNewUser(std::string username, std::string password, std::string email, int port, int clientsPublicKey, int clientsN, int token);