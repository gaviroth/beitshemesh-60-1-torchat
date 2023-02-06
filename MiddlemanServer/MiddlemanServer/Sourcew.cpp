// ConsoleApplication3.cpp : This file contains the 'main' function. Program execution begins and ends there.
#pragma warning(disable:4996) // for inet_addr(got a error)
#include <WinSock2.h>
#include <Windows.h>
#include "Sourcew.h"
#include <string>
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
long long int decrypt(int encrpyted_text)
{
	int d = private_key;
	long long int decrypted = 1;
	while (d--) {
		decrypted *= encrpyted_text;
		decrypted %= n;
	}
	return decrypted;
}
std::string decoder(std::vector<int> encoded)
{
	std::string s;
	// calling the decrypting function decoding function
	for (auto& num : encoded)
		s += decrypt(num);
	return s;
}
void handleNewClient(SOCKET a)
{
	std::cout << "in handleNewClient \n";
	char m[1024];
	recv(a, m, strlen(m), 0);//Takes a message from whoever send it to him and the goal is to get off the last port that is in this message and to change the length because of the change that has been done in the message
	std::cout << m << "\n";
	int  message_len = (int(m[1]) - 48) * 1000 + (int(m[2]) - 48) * 100 + (int(m[3]) - 48) * 10 + (int(m[4]) - 48);
	std::cout << m << "\n";
	std::vector<int> hector = {0};
	int minw=0;
	for (int i = 5; i < message_len; i++)
	{
			if (m[i] == ',')
		{
			hector.push_back(minw);
			minw = 0;
		}
		else
		{
			minw = minw * 10;
			minw = minw + int(m[i]) - 48;
		}
	}
	hector.push_back(minw);
	std::string new_msg = decoder(hector);
	int xy = message_len % 100 - 4;//Takes the two last numbers in the length and minus four because we delte the last port
	int ms_l = new_msg.length();

	int the_port = (int(new_msg[ms_l - 1]) - 48) * 1000 + (int(new_msg[ms_l - 2]) - 48) * 100 + (int(new_msg[ms_l - 3]) - 48) * 10 + (int(new_msg[ms_l - 4]) - 48);
	
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

	
	std::string leng = std::to_string(ms_l);
	if (leng.length() == 2)
		leng = "00" + leng;
	if (leng.length() == 3)
		leng = "0" + leng;
	std::string an = std::string(1, m[0]) + leng + new_msg;
	
	
	send(cv, an.c_str(), strlen(an.c_str()), 0);
	std::cout << "sent to: " << the_port << "\n";
	closesocket(cv);
}
