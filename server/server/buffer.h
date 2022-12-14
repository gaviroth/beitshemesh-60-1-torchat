#pragma once
#define PORT 1234
#define BUFFER_SIZE 1
#define MAX_BUFFER_SIZE 4

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
	MT_CLIENT_EXIT = 101,
	MT_CLIENT_ERROR = 102,
	MT_CLIENT_LOG_IN = 106,
	MT_CLIENT_LOG_OUT = 107,
	MT_CLIENT_SIGN_UP = 108,
};