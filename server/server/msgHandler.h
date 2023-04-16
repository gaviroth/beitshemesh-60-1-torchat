#pragma once
#include "buffer.h"

int randomid();
int generateid();
void eraseidFromSet(int id);
std::string returnJsonAsString();
void handelMsg(buffer bf, int port, int clientsPublicKey, int clientsN);