#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include "buffer.h"

void acceptClient();
void serve(int port);
void clientHandler(SOCKET clientSocket);