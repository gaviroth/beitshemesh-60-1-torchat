#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include "buffer.h"

void sendMsgToClient(std::string msg, int port);