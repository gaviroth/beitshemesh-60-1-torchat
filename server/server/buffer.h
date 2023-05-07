#pragma once
#define PORT 1234
#define BUFFER_SIZE 1
#define MAX_BUFFER_SIZE 6

#include <set>
#include <map>
#include <list>
#include <array>
#include <regex>
#include <thread>
#include <vector>
#include <chrono>
#include <string>
#include <sstream> 
#include <iostream>
#include "json.hpp"
#include <exception>
#include <algorithm>

using nlohmann::json;

typedef unsigned char byte;
typedef std::vector<unsigned char> buffer;

enum MessageType : byte
{
	MESSAGE_SENT_SUCCESSFULLY = 98,
	TOKEN_ISNT_VALID = 99,
	USER_BLOCKED_SUCCESSFULLY = 100,
	USER_UNBLOCKED_SUCCESSFULLY = 101,
	USER_DOSNT_EXIST = 102, 
	PASSWORD_DOSNT_MATCH = 103,
	USERNAME_ALREADY_TAKEN = 104,
	PASSWORD_NOT_VALID = 105,
	EMAIL_NOT_VALID = 106,
	MSG_ACK = 107,
	BLOCK_USER = 108,
	UNBLOCK_USER = 109,
	CLIENT_LOG_OUT = 110,
	CLIENT_SIGN_UP = 111,
	CLIENT_LOG_IN = 112,
	MSG_TO_CLIENT = 113,
	MSG_FROM_CLIENT = 114,
	CLIENT_LOGED_IN_SUCCESSFULLY = 115,
	CLIENT_SIGNED_UP_SUCCESSFULLY = 116,

};