#pragma once

#include <set>
#include <math.h>
#include <time.h>  
#include <vector>
#include <numeric>
#include <iostream>
#include <stdlib.h>    

void setkeys();
void primefiller();
int pickrandomprime();
int gcd(int a, int h);
void sendKeysToRouter();
long long int decrypt(int encrpyted_text);
std::string decoder(std::vector<int> encoded);
long long int encrypt(double message, int pkToEncodWith, int nToEncodWith);
std::vector<int> encoder(std::string message, int pkToEncodWith, int nToEncodWith);
