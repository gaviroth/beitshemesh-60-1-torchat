#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Windows.h>

#include <map>
#include <thread>
#include <vector>
#include <time.h> 
#include <string>
#include <iostream>
#include <stdlib.h>/* srand, rand */
#include "WSAInitializer.h"
#define PORT 2345

 std::map<std::string /*port*/,std::string /*public key*/> ms_ports;
 std::string spk;

 //Handles a new client connection on the provided socket.
void handleNewClient(SOCKET client_socket)
{
	// Receives the incoming data from the client.
	char data[1024];
	recv(client_socket, data, strlen(data), 0);

	// Checks if the incoming data is the server 
	if (data[0] == 's'&& data[1]=='p' && data[2] == 'k')
	{
		std::string port1 = "1234";
		std::string spk1 = std::string(data).substr(3, 8);
		
		spk = spk1;
	}
	// Checks if the incoming data is a middle man server
	if (data[0] == 'p' && data[1] == 'k')
	{
		std::string  port1 = std::string(data).substr(2, 4);
		
		std::string spk1 = std::string(data).substr(6, 8);
		
		std::cout << port1 << spk1;
		ms_ports.insert({ port1,spk1 });

	}
	else
	{
		// Generates three different random numbers between 0 and the numver of middle man servers
		std::srand(std::time(nullptr));
		int r1 = rand() % ms_ports.size() + 0;
		int r2 = rand() % ms_ports.size() + 0;
		int r3 = rand() % ms_ports.size() + 0;

		// Makes sure that the three random numbers generated are not equal to each other.
		while (r2 == r1)
		{
			r2 = rand() % ms_ports.size() + 0;
		}
		while (r1 == r3 || r2 == r3)
		{
			r3 = rand() % ms_ports.size() + 0;
		}
		// Checks if the incoming msg is the server.
		if (data[0] == 's')
		{
			// Constructs a path that includes the three randomly selected middle man servers.
			std::string qw;
			std::string qw1;
			std::string qw2;
			int i = 0;
			for (auto it = ms_ports.begin(); it != ms_ports.end(); it++)
			{
				if (it->second.length() == 2)
					it->second == "0000" + it->second;
				if (it->second.length() == 3)
					it->second == "000" + it->second;
				if (it->second.length() == 4)
					it->second == "00" + it->second;
				if (it->second.length() == 5)
					it->second == "0" + it->second;
				if (i == r1)

					qw = it->first + it->second;
				if (i == r2)
					qw1 = it->first + it->second;
				if (i == r3)
					qw2 = it->first + it->second;
				i++;
			}
			
			std::string q = qw + qw1 + qw2;
			std::cout << q;
			send(client_socket, q.c_str(), strlen(q.c_str()), 0);

		}
		// Checks if the incoming msg is a client.
		else if (data[0] == 'c')
		{
			// Constructs a path that includes the three randomly selected middle man servers.
			std::string qw;
			std::string qw1;
			std::string qw2;

			int i = 0;
			for (auto it = ms_ports.begin(); it != ms_ports.end(); it++)
			{
				if (it->second.length() == 2)
					it->second == "0000" + it->second;
				if (it->second.length() == 3)
					it->second == "000" + it->second;
				if (it->second.length() == 4)
					it->second == "00" + it->second;
				if (it->second.length() == 5)
					it->second == "0" + it->second;
				if (i == r1)
					qw = it->first+it->second ;
				if (i == r2)
					qw1 = it->first+it->second;
				if (i == r3)
					qw2 = it->first+it->second;
				i++;
			}
			std::string qw3 = "1234"+spk;//the server port....

			std::string q = qw3 + qw + qw1 + qw2;
			std::cout <<"\n \n \n msg: "<< q << "\n \n \n";
			send(client_socket, q.c_str(), strlen(q.c_str()), 0);
		}// making a mmesage that will include a path in case that the client asks for a path and the server is the last target....

		closesocket(client_socket);
	}
}

void startHandleRequests()
{
	SOCKET routerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (routerSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");

	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(PORT);// port that server will listen for
	sa.sin_family = AF_INET;// must be AF_INET
	sa.sin_addr.s_addr = INADDR_ANY;// when there are few ip's for the machine. We will use always "INADDR_ANY"

	// Connects between the socket and the configuration (port and etc..)
	if (::bind(routerSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");

	// Start listening for incoming requests of clients
	if (::listen(routerSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");


	while (true)
	{
		// this accepts the client and create a specific socket from server to this client
		// the process will not continue until a client connects to the server
		SOCKET client_socket = accept(routerSocket, NULL, NULL);
		if (client_socket == INVALID_SOCKET)
			throw std::exception(__FUNCTION__);

		// the function that handle the conversation with the client
		std::thread clientHandlerThread(handleNewClient, client_socket);
		clientHandlerThread.detach();

	}
}

int main()
{
	// Initialize the WSA API
	WSAInitializer wsaInit;

	startHandleRequests();
	return 0;
}