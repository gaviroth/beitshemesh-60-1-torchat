#pragma comment(lib, "ws2_32.lib")
#include "WSAInitializer.h"
#include "Sourcew.h"


int main()
{
	try
	{
		//mongocxx::instance instance{};
		WSAInitializer wsaInit;
		serve();
	}
	catch (std::exception& e)
	{
		std::cout << "Error occured: " << e.what() << std::endl;
	}

	system("PAUSE");
	return 0;
}

