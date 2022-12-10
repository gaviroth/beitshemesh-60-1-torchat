#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <thread>
#include <vector>
#include "WSAInitializer.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#define PORT 2345
static const char* port[5]={"1235", "1236", "1237", "1238", "1239"};
static const char server[4] = {'1','2', '3', '4'};
SOCKET bindAndListen()
{
	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(PORT);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = 0;
	SOCKET a = socket(AF_INET, SOCK_STREAM, 0);

	// again stepping out to the global namespace
	if (::bind(a, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");

	if (::listen(a, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
	return a;
}
void handleNewClient(SOCKET a)
{
	char m[1024];
	recv(a, m, strlen(m), 0);
	srand(time(NULL));
	int r1= rand() % 4 + 0;
	int r2 = rand() % 4 + 0;
	int r3 = rand() % 4 + 0;
	while (r2==r1)
	{
		r2 = rand() % 4 + 0;
	}
	while (r1==r3||r2==r3 )
	{
		r3 = rand() % 4 + 0;
	}
	if (m[0] == 's')
	{
		std::string qw=port[r1];
		std::string qw1 = port[r2];
		std::string qw2 = port[r3];
		std::string q = qw+qw1+qw2;
		send(a, q.c_str(), strlen(q.c_str()), 0);

	}
	else
	{
		std::string qw = port[r1];
		std::string qw1 = port[r2];
		std::string qw2 = port[r3];
		std::string qw3 = server;

		std::string q = qw3+qw + qw1 + qw2;
		send(a, q.c_str(), strlen(q.c_str()), 0);
	}


	closesocket(a);

}
void startHandleRequests()
{

	SOCKET a = bindAndListen();

	// create new thread for handeling message 


	while (true)
	{
		// the main thread is only accepting clients 
		// and add then to the list of handlers
		SOCKET client_socket = accept(a, NULL, NULL);
		if (client_socket == INVALID_SOCKET)
			throw std::exception(__FUNCTION__);
		// create new thread for client	and detach from it
		std::thread tr(handleNewClient, client_socket);
		tr.detach();

	}
}
int bro()
{

	// create new thread for handling message
	std::thread tr(startHandleRequests);
	tr.detach();
	std::string bup;
	while (true)
	{
		// the main thread is only accepting clients 
		// and add then to the list of handlers
		std::cin >> bup;
		std::cout << bup;
		if (bup == "EXIT")
			exit(0);

	}
	return 0;
}
int main()
{
	WSAInitializer wsaInit;
	bro();
	return 0;
}