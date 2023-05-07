#pragma once
#include "buffer.h"

int randomid();
int generateMsgid();
void handelMsg(buffer bf);
void msgReceived(buffer bf);
void eraseidFromSet(int msgid);
