// router.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma comment(lib, "ws2_32.lib")

#include <thread>
#include <vector>
#include <map>
#include <time.h> 
#include <iostream>
#include <stdlib.h>/* srand, rand */
#include "WSAInitializer.h"
#include <string>
#define PORT 2345

 std::map<std::string /*port*/,std::string /*public key*/> ms_ports;
 std::string spk;

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
	if (m[0] == 's'&&m[1]=='p' && m[2] == 'k')
	{
		std::string port1 = "1234";
		std::string spk1 = std::string(m).substr(3, 8);
		
		spk = spk1;
	}
	if (m[0] == 'p' && m[1] == 'k')
	{
		std::string  port1 = std::string(m).substr(2, 4);
		
		std::string spk1 = std::string(m).substr(6, 8);
		
		std::cout << port1 << spk1;
		ms_ports.insert({ port1,spk1 });

	}
	else
	{
		srand(time(NULL));
		int r1 = rand() % ms_ports.size() + 0;
		int r2 = rand() % ms_ports.size() + 0;
		int r3 = rand() % ms_ports.size() + 0;
		while (r2 == r1)
		{
			r2 = rand() % ms_ports.size() + 0;
		}
		while (r1 == r3 || r2 == r3)
		{
			r3 = rand() % ms_ports.size() + 0;
		}//r1,r2 and r3 are thrre diffrent random numbers betwwen 0 to 4;
		if (m[0] == 's')
		{
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
			send(a, q.c_str(), strlen(q.c_str()), 0);

		}// making a mmesage that will include a path in case that the server asks for a path and than he wiil need to add his target in the end
		else if (m[0] == 'c')
		{
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
			send(a, q.c_str(), strlen(q.c_str()), 0);
		}// making a mmesage that will include a path in case that the client asks for a path and the server is the last target....


		closesocket(a);
	}
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
int router()
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
	router();
	return 0;
}
