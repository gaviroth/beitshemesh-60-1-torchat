#include "tokenHandler.h" 
#include "MongoDatabase.h"

#include <set>
#include <ctime>
#include <string>
#include <cstdlib>

extern std::mutex tokenMtx;
extern std::set<int> tokenSet;

// function returns a random 6 digit token 
int randomToken() 
{
	std::srand(std::time(nullptr));

	int num1 = 0;
	int num2 = 0;
	int num3 = 0;
	int num4 = 0;
	int num5 = 0;
	int num6 = 0;
	int token = 0;

	num1 = (rand() % 9) + 1;
	num2 = (rand() % 9) + 1;
	num3 = (rand() % 9) + 1;
	num4 = (rand() % 9) + 1;
	num5 = (rand() % 9) + 1;
	num6 = (rand() % 9) + 1;

	token = num1 * 100000;
	token += num2 * 10000;
	token += num3 * 1000;
	token += num4 * 100;
	token += num5 * 10;
	token += num6;

	return token;
}

/*Function generates a token, makes sure it's
not taken by another user and saves it to Mongo dB 
(because the next request will be to a different thread) 
and saves to the set so you can make sure token isn't taken*/
int generateToken()
{
	int token = 0;
	std::set<int>::iterator it;

	tokenMtx.lock();
	do {
		token = randomToken();
		it = tokenSet.find(token);
	} while (it != tokenSet.end());

	tokenSet.insert(token);
	tokenMtx.unlock();

	return token;
}

bool isTokenValid(std::string username, int token) 
{
	if (!isUserInactiveForHalfHour(username))
	{
		if (isUserActive(username))
		{
			if (doesTokenMatch(token, username)) {
				return true;
			}
		}
	}
	return false;
}

void eraseTokenFromSet(int token)
{
	tokenMtx.lock();
	tokenSet.erase(token);
	tokenMtx.unlock();
}