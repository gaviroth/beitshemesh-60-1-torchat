#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include "buffer.h"

std::string getRout();// gets rout from router
buffer strToBuffer(std::string str);//turns string into buffer 
buffer putMsgTougther(std::string msg, int statusCode);// puts msg togther by protocol
void sendMsgToClient(std::string msg, int port, int statusCode);// handells sending msg to client