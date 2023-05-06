#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include "buffer.h"
#include "Encryption.h"

std::string getRout();// gets rout from router
std::string vectorToString(std::vector<int> vec);
buffer strToBuffer(std::string str);//turns string into buffer 
std::string putMsgTougther(std::string msg, int statusCode);// puts msg togther by protocol
void sendMsgToClient(std::string msg, int port, int clientsPublicKey, int clientsN, int msgCode);// handells sending msg to client