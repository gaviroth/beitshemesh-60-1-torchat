#pragma once
#include "mongoInclude.h"
#include "buffer.h"
#include <chrono>
#include <mutex>
#include <ctime>
using nlohmann::json;

void creat();
bool isUserActive(std::string username);
bool doesUserExist(std::string username);
bool updateStatusToOffline(std::string username);
bool updateToken(int Token, std::string username);
bool doesTokenMatch(int Token, std::string username);
bool isUserInactiveForHalfHour(std::string userName);
bool doesPasswordMatch(std::string username, std::string password);
bool updateUsersInfo(std::string userName, int port, int clientsPublicKey, int clientsN, int token);
void addNewUser(std::string username, std::string password, std::string email, int port, int clientsPublicKey, int clientsN, int token);