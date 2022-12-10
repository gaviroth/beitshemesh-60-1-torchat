#pragma comment(lib, "ws2_32.lib")

#include "mongoInclude.h"
#include "MongoDatabase.h"

#include "buffer.h"
#include "Server.h"
#include "WSAInitializer.h"

int main()
{
    try
    {
        mongocxx::instance instance{};
        creat();//mongodb
        //addNewUser("username", "password", "email");

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