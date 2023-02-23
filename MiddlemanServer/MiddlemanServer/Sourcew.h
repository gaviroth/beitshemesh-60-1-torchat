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
static std::set<int> prime; // a set will be the collection of prime numbers,
       // where we can select random primes p and q
static int public_key;
static int private_key;
static int n;
