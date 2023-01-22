#pragma once
#include "buffer.h"

int randomToken();
void eraseTokenFromSet(int token);
int generateToken(std::string userName);
bool isTokenValid(std::string userName, int token);