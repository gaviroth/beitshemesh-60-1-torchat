#pragma once
#include "buffer.h"
#include <mutex>

int randomToken();
int generateToken();
void eraseTokenFromSet(int token);
bool isTokenValid(std::string username, int token);