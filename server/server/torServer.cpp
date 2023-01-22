#pragma comment(lib, "ws2_32.lib")

#include "mongoInclude.h"
#include "MongoDatabase.h"

#include <set>
#include "buffer.h"
#include "Server.h"
#include "tokenHandler.h" 
#include "WSAInitializer.h"
#include "sendMsgToClientHandler.h"

std::set<int> mySet;

int main()
{
    try
    {
        mongocxx::instance instance{};
        creat();//mongodb

        WSAInitializer wsaInit;
        serve(1234);
    }
    catch (std::exception &e)
    {
        std::cout << "Error occured: " << e.what() << std::endl;
    }

    system("PAUSE");
    return 0;
}