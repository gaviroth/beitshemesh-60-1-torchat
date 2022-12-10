#pragma once
#include "mongoInclude.h"
#include "buffer.h"

using nlohmann::json;
void creat();
bool doesUserExist(std::string username);
bool doesPasswordMatch(std::string username, std::string password);
void addNewUser(std::string username, std::string password, std::string email);