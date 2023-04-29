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
	MSG_ACK = 107,
	CLIENT_EXIT = 108,
	CLIENT_ERROR = 109,
	CLIENT_LOG_OUT = 110,
	CLIENT_SIGN_UP = 111,
	CLIENT_LOG_IN = 112,
	MSG_TO_CLIENT = 113,
	MSG_FROM_CLIENT = 114,
	CLIENT_LOG_IN_RESPONSE = 115,
	CLIENT_SIGN_UP_RESPONSE = 116,
};