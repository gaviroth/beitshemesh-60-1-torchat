#include "sendMsgToClientHandler.h"
#pragma warning(disable:4996) // for inet_addr(got a error)


SOCKET _clientSocket;
std::string serverIP = "127.0.0.1";

void sendMsgToClient(std::string msg, int port) {

	// we connect to client that uses TCP. thats why SOCK_STREAM & IPPROTO_TCP
	_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (_clientSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");

	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(port); // port that server will listen to
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = inet_addr(serverIP.c_str());

	// the process will not continue until the server accepts the client
	int status = connect(_clientSocket, (struct sockaddr*)&sa, sizeof(sa));

	if (status == INVALID_SOCKET)
		throw std::exception("Cant connect to client");

	send(_clientSocket, msg.c_str(), msg.size(), 0);  // last parameter: flag. for us will be 0. 

	closesocket(_clientSocket);
}