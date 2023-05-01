#include "Server.h"
#include "Encryption.h"
#include "LoginRequestHandler.h"
#include "SignUpRequestHandler.h"
#include "sendMsgToClientHandler.h"

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
		buffer portV;
		buffer clientsNV;
		buffer clientsPublicKeyV;
		int temp = 0;
		int port = 0;
		int Ctemp = 0;
		bool flag = true;
		int clientsN = 0;
		int clientsPublicKey = 0;
		unsigned int dataLen = 0;
		int msgLenAfterdecode = 0;
		std::string decodedmsg = "";
		std::vector<int> encodedmsg = { 0 };
		char* data = new char[BUFFER_SIZE + MAX_BUFFER_SIZE];
		int msg = recv(clientSocket, data, BUFFER_SIZE + MAX_BUFFER_SIZE, 0);

		unsigned int msgCode = static_cast<unsigned int>(data[0]);//byte 0 is msg code

		dataLen = (int(data[1]) - 48) * 100000 + (int(data[2]) - 48) * 10000 + (int(data[3]) - 48) * 1000 + (int(data[4]) - 48) * 100 + (int(data[5]) - 48) * 10 + (int(data[6]) - 48);//take 1-4 byts and turn them from char to int to have the data lenght
		
		data = new char[dataLen];//rests data so you can receive json

		msg = recv(clientSocket, data, dataLen, 0);//receive the rest of thr message
		if (msg == INVALID_SOCKET)//check to make sure msg is valid
		{
			std::string str = "Error while recieving message from socket: ";
			str += std::to_string(clientSocket);
			throw std::exception(str.c_str());
		}

		for (int i = 0; i <= dataLen; i++)// loop puts msg in vector so you can decod msg
		{
			if (data[i] == ',')// if char is , push last number into vector
			{
				
				encodedmsg.push_back(Ctemp);
				Ctemp = 0;
			}
			else 
			{
				Ctemp = Ctemp * 10;// if number has multple digits 
				Ctemp = Ctemp + (int(data[i]) - 48); // turn char to int and add to number if it has multple digits 
			}
		}

		decodedmsg = decoder(encodedmsg);// decode msg 
		std::cout << decodedmsg;
		msgLenAfterdecode = decodedmsg.length();
		const char* dataDecodedmsg = decodedmsg.c_str();// turn msg str to char*

		//loop puts json into buffer(bf)
		for (int i = 0; i < msgLenAfterdecode && flag; i++)
		{
			if (dataDecodedmsg[i] == '}') {
				flag = false;
			}
			bf.push_back(static_cast<unsigned char>(dataDecodedmsg[i])); 
			temp = i + 1;

		}

		if (msgCode == CLIENT_SIGN_UP || msgCode == CLIENT_LOG_IN) //if msg is sign up or log in take port public key and n 
		{
		    flag = true;//reset flag
			//loop takes clients port
			for (int i = temp; i < msgLenAfterdecode && flag; i++)// i = temp because we need the next chars after the json
			{
				if (temp + 2 < i) // less then 2 so the loop will run 4 times because a port is 4 digits 
				{
						flag = false;
				}
				portV.push_back(static_cast<unsigned char>(dataDecodedmsg[i]));

			}
			std::string portString(portV.begin(), portV.end());
			std::cout << portString;
			port = stoi(portString);
			//std::cout << port << "\n";

			flag = true;//reset flag
			//loop takes clients public key
			for (int i = temp + 4; i < msgLenAfterdecode && flag; i++)// temp + 4 because a port is 4 digits and we neet the next 2 
			{
				if (temp + 4 < i) // less then 4 because the port is 4 digits, so it will run 2 times because the clients Public Key is 2 digits
				{
					flag = false;
				}
				clientsPublicKeyV.push_back(static_cast<unsigned char>(dataDecodedmsg[i]));

			}
			std::string clientsPublicKeyString(clientsPublicKeyV.begin(), clientsPublicKeyV.end());
			clientsPublicKey = stoi(clientsPublicKeyString);
			//std::cout << clientsPublicKey << "\n";

			flag = true;//reset flag
			//loop takes clients n
			for (int i = temp + 6; i < msgLenAfterdecode && flag; i++)//temp + 6 because a port is 4 digits and the clients Public Key is 2 digits and we need the next 6
			{
				if (temp + 10 < i) // less then 10 because the port is 4 digits and the clients Public Key is 2 digits, so it will run 6 times because the clients n is 6 digits
				{
					flag = false;
				}
				clientsNV.push_back(static_cast<unsigned char>(dataDecodedmsg[i]));

			}
			std::string clientsNString(clientsNV.begin(), clientsNV.end());
			clientsN = stoi(clientsNString);
			//std::cout << clientsN << "\n";
		}

		switch (msgCode) // send msg to the relevant handler based on msg code 
		{
		case 111://SIGN UP
			signup(bf, port, clientsPublicKey, clientsN);
			break;
		case 112://LOG IN
			login(bf, port, clientsPublicKey, clientsN);
			break;
		case 113://MSG
			signup(bf, port, clientsPublicKey, clientsN);
			break;
		case 114:
			signup(bf, port, clientsPublicKey, clientsN);
			break;
		default:
			signup(bf, port, clientsPublicKey, clientsN);
		}
		closesocket(clientSocket);
		
	}
	catch (const std::exception& e)
	{
		std::cout << "Error occured: " << e.what() << std::endl;
		//closesocket(clientSocket);
	}
}