#pragma comment(lib, "ws2_32.lib")
#include "WSAInitializer.h"
#include "Sourcew.h"


int main(int argc, char* argv[])
{
	try
	{
		//mongocxx::instance instance{};
		WSAInitializer wsaInit;
		int num= (int(argv[1][0]) - 48) * 1000 + (int(argv[1][1]) - 48) * 100 + (int(argv[1][2]) - 48) * 10 + (int(argv[1][3]) - 48);
		serve(num);
	}
	catch (std::exception& e)
	{
		std::cout << "Error occured: " << e.what() << std::endl;
	}

	system("PAUSE");
	return 0;
}

