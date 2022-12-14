 // ConsoleApplication3.cpp : This file contains the 'main' function. Program execution begins and ends there.
#pragma warning(disable:4996) // for inet_addr(got a error)
#include <WinSock2.h>
#include <Windows.h>
#include "Sourcew.h"

SOCKET cv;
SOCKET _serverSocket;
std::string serverIP = "127.0.0.1";

void serve(int PORT)
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
	std::cout << "in handleNewClient \n";
	char m[1024];
	recv(a, m, strlen(m), 0);//Takes a message from whoever send it to him and the goal is to get off the last port that is in this message and to change the length because of the change that has been done in the message
	std::cout << m << "\n";
	int  message_len = (int(m[1]) - 48) * 1000 + (int(m[2]) - 48) * 100 + (int(m[3]) - 48) * 10 + (int(m[4]) - 48);
	std::cout << m << "\n";
	int xy = message_len % 100 - 4;//Takes the two last numbers in the length and minus four because we delte the last port
	int the_port = (int(m[message_len + 1]) - 48) * 1000 + (int(m[message_len  + 2]) - 48) * 100 + (int(m[message_len + 3]) - 48) * 10 + (int(m[message_len + 4]) - 48);
	std::cout << the_port << "\n";

	cv = socket(AF_INET, SOCK_STREAM, 0);

	if (cv == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");

	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(the_port);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(serverIP.c_str());

	int status = connect(cv, (struct sockaddr*)&sa, sizeof(sa));

	if (status == INVALID_SOCKET)
		throw std::exception("Cant connect to client");

	std::cout << "connected to: " << the_port << "\n";

	char answer[1024];
	for (auto it = 0; it < 1024; it++)
	{
		answer[it] = m[it];
	}
	answer[message_len + 1] = 0;//aditing the message...
	answer[message_len + 2] = 0;
	answer[message_len + 3] = 0;
	answer[message_len + 4] = 0;

	answer[3] = (xy / 10 + 48);
	answer[4] = (xy % 10 + 48);
	send(cv, answer, strlen(answer), 0);
	std::cout << "sent to: " << the_port << "\n";
	closesocket(cv);
}