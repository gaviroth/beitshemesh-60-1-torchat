#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include "WSAInitializer.h"
void acceptClient();
void serve(int PORT);
void handleNewClient(SOCKET a);