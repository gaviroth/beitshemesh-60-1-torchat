 // ConsoleApplication3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <WinSock2.h>
#include <Windows.h>
#include "Sourcew.h"
SOCKET _serverSocket;
#define PORT 1235
void serve()
{
	// this server use TCP. that why SOCK_STREAM & IPPROTO_TCP
	// if the server use UDP we will use: SOCK_DGRAM & IPPROTO_UDP
	_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (_serverSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");

	/*try
	{
		// the only use of the destructor should be for freeing
		// resources that was allocated in the constructor
		closesocket(_serverSocket);
	}
	catch (...) {}
	*/

	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(PORT); // port that server will listen for
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = INADDR_ANY;    // when there are few ip's for the machine. We will use always "INADDR_ANY"

	// Connects between the socket and the configuration (port and etc..)
	if (bind(_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");

	// Start listening for incoming requests of clients
	if (listen(_serverSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
	std::cout << "Listening on port " << PORT << std::endl;

	while (true)
	{
		// the main thread is only accepting clients 
		// and add then to the list of handlers
		std::cout << "Waiting for client connection request" << std::endl;
		acceptClient();
	}
}


void acceptClient()
{

	// this accepts the client and create a specific socket from server to this client
	// the process will not continue until a client connects to the server
	SOCKET client_socket = accept(_serverSocket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);

	std::cout << "Client accepted. Server and client can speak" << std::endl;
	// the function that handle the conversation with the client

	std::thread clientHandlerThread(handleNewClient, client_socket);
	clientHandlerThread.detach();

}
void handleNewClient(SOCKET a)
{
	char m[1024];
	recv(a, m, strlen(m), 0);
	int x = (int(m[2]) - 48) * 1000 + (int(m[3]) - 48) * 100 + (int(m[4]) - 48) * 10 + (int(m[5]) - 48);
	int xy = x % 100 - 4;
	int co = (int(m[x - 4]) - 48) * 1000 + (int(m[x - 3]) - 48) * 100 + (int(m[x - 2]) - 48) * 10 + (int(m[x - 1]) - 48);
	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(co);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = 0;
	SOCKET cv = socket(AF_INET, SOCK_STREAM, 0);
	char answer[1024];
	for (auto it = 0; it < 1024; it++)
	{
		answer[it] = m[it];
	}
	answer[x - 4] = 0;
	answer[x - 3] = 0;
	answer[x - 2] = 0;
	answer[x - 1] = 0;
	answer[4] = xy / 10;
	answer[5] = xy % 10;
	send(cv, answer, strlen(answer), 0);
	closesocket(cv);


}