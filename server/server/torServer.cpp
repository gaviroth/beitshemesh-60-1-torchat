#pragma comment(lib, "ws2_32.lib")

#include "mongoInclude.h"
#include "MongoDatabase.h"

#include <set>
#include <mutex>
#include "buffer.h"
#include "Server.h"
#include "Encryption.h"
#include "tokenHandler.h" 
#include "WSAInitializer.h"
#include "sendMsgToClientHandler.h"

int n;
int public_key;
int private_key;
std::mutex idMtx;
std::mutex tokenMtx;
std::mutex mongoMtx;
std::set<int> idSet;
std::set<int> prime;
std::set<int> tokenSet;
mongocxx::client cli;
mongocxx::database db;
mongocxx::uri uri{ "mongodb://localhost:27017" };

int main()
{
    try
    {
        // Initialize the WSA API
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