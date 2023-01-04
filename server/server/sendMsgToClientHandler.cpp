#include "sendMsgToClientHandler.h"
#pragma warning(disable:4996) // for inet_addr(got a error)


SOCKET _clientSocket;
SOCKET _routerSocket;
std::string serverIP = "127.0.0.1";

std::string getRout() {
	char ports[12];
	std::string msg = "s";
	std::string sports = "";

	// we connect to client that uses TCP. thats why SOCK_STREAM & IPPROTO_TCP
	_routerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (_routerSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");

	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(2345); // port that server will listen to
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = inet_addr(serverIP.c_str());

	// the process will not continue until the server accepts the client
	int status = connect(_routerSocket, (struct sockaddr*)&sa, sizeof(sa));

	if (status == INVALID_SOCKET)
		throw std::exception("Cant connect to client");

	send(_routerSocket, msg.c_str(), msg.size(), 0);  // last parameter: flag. for us will be 0. 
	recv(_routerSocket, ports, 12, 0);
	closesocket(_routerSocket);

	for (int i = 0; i < 12; i++) {
		sports = sports + ports[i];
	}
	return sports;
}

buffer putMsgTougther(std::string msg, int statusCode) {
	buffer dataAns;
	std::stringstream dataLenStr{};

	byte code = statusCode;
	buffer ans = { code };//first byte is message code

	std::string Str = msg;
	unsigned int msgLen = Str.length();
	if (msgLen < 1000) {//if msg is less then 4 bytes add 0 to make it 4 byte without changing lan
		if (msgLen < 100) {
			if (msgLen < 10) {
				dataLenStr << msgLen;// put length in string stream
				std::string dataLenStrStr = dataLenStr.str();//put length in string 
				dataLenStrStr = "000" + dataLenStrStr;
				const char* dataLenAsCharArray = dataLenStrStr.c_str();//put length in c string
				unsigned int sizeOfDataLenStrStr = dataLenStrStr.length(); //saves number of bytes 
				ans.insert(ans.end(), dataLenAsCharArray, dataLenAsCharArray + sizeOfDataLenStrStr);//put length in buffer
			}
			else {
				dataLenStr << msgLen;// put length in string stream
				std::string dataLenStrStr = dataLenStr.str();//put length in string 
				dataLenStrStr = "00" + dataLenStrStr;
				const char* dataLenAsCharArray = dataLenStrStr.c_str();//put length in c string
				unsigned int sizeOfDataLenStrStr = dataLenStrStr.length(); //saves number of bytes 
				ans.insert(ans.end(), dataLenAsCharArray, dataLenAsCharArray + sizeOfDataLenStrStr);//put length in buffer
			}
		}
		else {
			dataLenStr << msgLen;// put length in string stream
			std::string dataLenStrStr = dataLenStr.str();//put length in string 
			dataLenStrStr = "0" + dataLenStrStr;
			const char* dataLenAsCharArray = dataLenStrStr.c_str();//put length in c string
			unsigned int sizeOfDataLenStrStr = dataLenStrStr.length(); //saves number of bytes 
			ans.insert(ans.end(), dataLenAsCharArray, dataLenAsCharArray + sizeOfDataLenStrStr);//put length in buffer
		}
	}
	else {
		dataLenStr << msgLen;// put length in string stream
		std::string dataLenStrStr = dataLenStr.str();//put length in string 
		const char* dataLenAsCharArray = dataLenStrStr.c_str();//put length in c string
		unsigned int sizeOfDataLenStrStr = dataLenStrStr.length(); //saves number of bytes 
		ans.insert(ans.end(), dataLenAsCharArray, dataLenAsCharArray + sizeOfDataLenStrStr);//put length in buffer
	}
	dataAns = strToBuffer(Str); // put data in buffer
	ans.insert(ans.end(), dataAns.begin(), dataAns.end());//insert data at the end of the buffer

	return ans;
}

void sendMsgToClient(std::string msg, int port, int statusCode) {
	buffer ans;
	char temp = ' ';
	int mdlserverPort = 0; 
	std::string ports = "";

	ports = getRout();//get rout

	std::string cport = "";
	cport = std::to_string(port);
	ports = cport + ports;// put clients port at at the beging of ports

	// get first port in int
	temp = ports.back();
	ports = ports.substr(0, ports.length() - 1);
	mdlserverPort = (int(temp)- 48);

	temp = ports.back();
	ports = ports.substr(0, ports.length() - 1);
	mdlserverPort = mdlserverPort + ((int(temp) - 48) * 10);

	temp = ports.back();
	ports = ports.substr(0, ports.length() - 1);
	mdlserverPort = mdlserverPort + ((int(temp) - 48) * 100);

	temp = ports.back();
	ports = ports.substr(0, ports.length() - 1);
	mdlserverPort = mdlserverPort + ((int(temp) - 48) * 1000);

	msg = msg + ports;
	ans = putMsgTougther(msg, statusCode);

	std::string strans(ans.begin(), ans.end());//turn to str to send to client (only turned to buffer to put msg togther)

	// we connect to client that uses TCP. thats why SOCK_STREAM & IPPROTO_TCP
	_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (_clientSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");

	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(mdlserverPort); // port that server will listen to
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = inet_addr(serverIP.c_str());

	// the process will not continue until the server accepts the client
	int status = connect(_clientSocket, (struct sockaddr*)&sa, sizeof(sa));

	if (status == INVALID_SOCKET)
		throw std::exception("Cant connect to client");

	send(_clientSocket, strans.c_str(), strans.size(), 0);  // last parameter: flag. for us will be 0. 

	closesocket(_clientSocket);
}

buffer strToBuffer(std::string str)//turns string into buffer 
{
	buffer ans;
	for (int i = 0; i < str.size(); i++)
	{
		ans.push_back(static_cast<unsigned char>(str.at(i)));
	}

	return ans;
}