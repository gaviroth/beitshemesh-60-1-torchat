#include "Server.h"
#include "LoginRequestHandler.h"
#include "SignUpRequestHandler.h"

SOCKET _serverSocket;

void serve(int port)
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
	sa.sin_port = htons(port); // port that server will listen for
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = INADDR_ANY;    // when there are few ip's for the machine. We will use always "INADDR_ANY"

	// Connects between the socket and the configuration (port and etc..)
	if (bind(_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");

	// Start listening for incoming requests of clients
	if (listen(_serverSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
	std::cout << "Listening on port " << port << std::endl;

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
	
	std::thread clientHandlerThread(clientHandler, client_socket);
	clientHandlerThread.detach();

}


void clientHandler(SOCKET clientSocket)
{
	try
	{
		buffer bf;
		buffer port;
		int temp = 0;
		bool flag = true;
		char* data = new char[BUFFER_SIZE + MAX_BUFFER_SIZE];
		int msg = recv(clientSocket, data, BUFFER_SIZE + MAX_BUFFER_SIZE, 0);

		char* tempC = nullptr;
		unsigned int  dataLen = 0;
		unsigned int  tempCSize = 0;

		unsigned int msgCode = static_cast<unsigned int>(data[0]);

		for (int i = 1; i <= sizeof(unsigned int) + BUFFER_SIZE && flag; i++)//loop checks if part of the json is sent in datalen and if so it stores the json data inside tempC
		{
			if (data[i] == '{')
			{
				if (i != sizeof(unsigned int) + BUFFER_SIZE)
				{
					tempC = new char[(sizeof(unsigned int) + BUFFER_SIZE) - i];
					for (int j = i; j < sizeof(unsigned int) + BUFFER_SIZE; j++)
					{
						tempC[tempCSize] = data[j];
						tempCSize++;
					}
				}
				flag = false;
			}

			else
			{
				dataLen += static_cast<unsigned int>(data[i]);
			}
		}

		data = new char[dataLen - tempCSize];//rests data so you can receive json

		msg = recv(clientSocket, data, dataLen - tempCSize, 0);//receive the rest of thr message
		if (msg == INVALID_SOCKET)
		{
			std::string str = "Error while recieving message from socket: ";
			str += std::to_string(clientSocket);
			throw std::exception(str.c_str());
		}

		flag = true;
		if (tempC != nullptr)//checks if datalen took part of the json
		{
			bf = buffer(tempC, tempC + tempCSize);
		}

		for (size_t i = 0; i < (dataLen - tempCSize) && flag; i++)//puts json into buffer(bf)
		{
			if (data[i] == '}') {
				flag = false;
			}
			bf.push_back(static_cast<unsigned char>(data[i]));
			temp = i + 1;

		}
		flag = true;

		for (size_t i = temp; i < (dataLen - tempCSize) && flag; i++)//puts json into buffer(bf)
		{
			if (temp + 10 < i ) {
				flag = false;
			}
			port.push_back(static_cast<unsigned char>(data[i]));

		}

		switch (msgCode) {
		case 111://SIGN UP
			signup(bf, port);
			break;
		case 112://LOG IN
			login(bf, port);
			break;
		case 113:
			signup(bf, port);
			break;
		case 114:
			signup(bf, port);
			break;
		default:
			signup(bf, port);
		}
		closesocket(clientSocket);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error occured: " << e.what() << std::endl;
		//closesocket(clientSocket);
	}
}