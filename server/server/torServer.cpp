#pragma comment(lib, "ws2_32.lib")

#include "mongoInclude.h"
#include "MongoDatabase.h"

#include <set>
#include "buffer.h"
#include "Server.h"
#include "Encryption.h"
#include "tokenHandler.h" 
#include "WSAInitializer.h"
#include "sendMsgToClientHandler.h"

int n;
int public_key;
int private_key;
std::set<int> mySet;
std::set<int> prime;

int main()
{
    try
    {
        WSAInitializer wsaInit;

        primefiller();//fill primes for encryption
        setkeys();//set keys for encryption
        sendKeysToRouter();

        mongocxx::instance instance{};
        creat();//mongodb

        serve(1234);
    }
    catch (std::exception &e)
    {
        std::cout << "Error occured: " << e.what() << std::endl;
    }

    system("PAUSE");
    return 0;
}