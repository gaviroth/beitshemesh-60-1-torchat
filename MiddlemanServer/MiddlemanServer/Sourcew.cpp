//// ConsoleApplication3.cpp : This file contains the 'main' function. Program execution begins and ends there.
#pragma warning(disable:4996) // for inet_addr(got a error)
#include <WinSock2.h>
#include <Windows.h>
#include "Sourcew.h"
#include <set>
#include <string>

SOCKET cv;
SOCKET _serverSocket;
extern std::string serverIP;

extern std::set<int> prime; // a set will be the collection of prime numbers,
	   // where we can select random primes p and q
extern int public_key;
extern int private_key;
extern int n;

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
long long int decrypt(long long int encrpyted_text)
{
	int d = private_key;
	long long int decrypted = 1;
	while (d--) {
		decrypted *= encrpyted_text;
		decrypted %= n;
	}
	return decrypted;
}
std::string decoder(std::vector<long long int> encoded)
{
	std::string s;
	// calling the decrypting function decoding function
	for (auto& num : encoded)
		s += decrypt(num);
	return s;
}
void handleNewClient(SOCKET client_socket)
{
	char* code = new char[7];
	recv(client_socket, code, 7, 0);
	int  message_len = (int(code[1]) - 48) * 100000 + (int(code[2]) - 48) * 10000 + (int(code[3]) - 48) * 1000 + (int(code[4]) - 48) * 100 + (int(code[5]) - 48) * 10 + (int(code[6]) - 48);

	std::cout << "in handleNewClient \n";
	std::cout << message_len << "\n";
	char* m = new char[message_len];
	recv(client_socket, m, message_len, 0);//Takes a message from whoever send it to him and the goal is to get off the last port that is in this message and to change the length because of the change that has been done in the message
	std::cout << m << "\n";
	for (int i = 0; i < message_len; i++)
	{
		std::cout << m[i];
	}
	std::cout << m << "\n";
	std::cout << message_len << "\n";
	std::vector<long long int> hector = { 0 };
	long long int minw = 0;
	for (int i = 0; i < message_len; i++)
	{
		if (m[i] == ',')
		{
			hector.push_back(minw);
			minw = 0;
		}
		else
		{
			minw = minw * 10;
			minw = minw + long long int(m[i]) - 48;
		}
	}
	std::cout << "loop" << "\n";
	hector.push_back(minw);
	std::string new_msg = decoder(hector);
	new_msg = new_msg.substr(1);
	std::cout << "nmsg:" << new_msg << "\n";
	std::cout << "decod" << "\n";
	//int xy = message_len % 100 - 4;//Takes the two last numbers in the length and minus four because we delte the last port
	int ms_l = new_msg.length();
	std::cout << new_msg << "\n";
	int the_port = (int(new_msg[ms_l - 1]) - 48) + (int(new_msg[ms_l - 2]) - 48) * 10 + (int(new_msg[ms_l - 3]) - 48) * 100 + ((int(new_msg[ms_l - 4]) - 48) * 1000);
	new_msg = new_msg.substr(0, new_msg.length() - 4);
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

	ms_l = ms_l - 4;
	std::string leng = std::to_string(ms_l);
	if (leng.length() == 2)
		leng = "0000" + leng;
	if (leng.length() == 3)
		leng = "000" + leng;
	if (leng.length() == 4)
		leng = "00" + leng;
	if (leng.length() == 5)
		leng = "0" + leng;
	std::string an = std::string(1, code[0]) + leng + new_msg;
	std::cout << "nmsg:" << new_msg << "\n";
	std::cout << "msg: " << an << "\n";

	send(cv, an.c_str(), strlen(an.c_str()), 0);
	std::cout << "sent to: " << the_port << "\n";
	closesocket(cv);
}

