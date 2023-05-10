#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <set>

#include "WSAInitializer.h"

void acceptClient();
void serve(int PORT);
void handleNewClient(SOCKET a);
long long int decrypt(long long int encrpyted_text);
std::string decoder(std::vector<long long int> encoded);
