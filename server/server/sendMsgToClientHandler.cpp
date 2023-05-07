#include "sendMsgToClientHandler.h"
#pragma warning(disable:4996) // for inet_addr(got a error)

extern std::set<int> mySet;

std::string getRout() {

	SOCKET _routerSocket;
	std::string serverIP = "127.0.0.1";

	char mdlmServers[36];
	std::string msg = "s";
	std::string SmdlmServers = "";

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
	recv(_routerSocket, mdlmServers, 36, 0);
	closesocket(_routerSocket);

	for (int i = 0; i < 36; i++) {
		SmdlmServers = SmdlmServers + mdlmServers[i];
	}
	return SmdlmServers;
}

std::string putMsgTougther(std::string msg, int statusCode)
{
	std::string ans = "";

	char codeAsChar = static_cast<char>(statusCode);
	ans.push_back(codeAsChar);

	std::string dataLenStrStr = std::to_string(msg.length());//put length in string 

	if (dataLenStrStr.length() == 2)
		dataLenStrStr = "0000" + dataLenStrStr;
	if (dataLenStrStr.length() == 3)
		dataLenStrStr = "000" + dataLenStrStr;
	if (dataLenStrStr.length() == 4)
		dataLenStrStr = "00" + dataLenStrStr;
	if (dataLenStrStr.length() == 5)
		dataLenStrStr = "0" + dataLenStrStr;

	ans = ans + dataLenStrStr + msg;

	return ans;
}

void sendMsgToClient(std::string msg, int port, int clientsPublicKey, int clientsN, int msgCode) {

	SOCKET _clientSocket;
	std::string serverIP = "127.0.0.1";

	std::vector<int> encodedMsg;
	std::string encodedMsgAsStr = " ";

	char temp = ' ';
	int tempN = 0;
	int tempPk = 0;
	int tempPort = 0; 
	int mdlmanServerPort = 0;

	std::string ans = "";
	std::string Rout = "";

	Rout = getRout();//get rout and kyes and Ns

	encodedMsg = encoder(msg, clientsPublicKey, clientsN);
	encodedMsgAsStr = vectorToString(encodedMsg);
	encodedMsgAsStr = encodedMsgAsStr + std::to_string(port);

	for (int i = 0; i < 3; i++) {
		tempPort = std::stoi(Rout.substr(0, 4));
		Rout.erase(0, 4);

		tempPk = std::stoi(Rout.substr(0, 2));
		Rout.erase(0, 2);

		tempN = std::stoi(Rout.substr(0, 6));
		Rout.erase(0, 6);

		encodedMsg = encoder(encodedMsgAsStr, tempPk, tempN);
		encodedMsgAsStr = vectorToString(encodedMsg);
		encodedMsgAsStr = encodedMsgAsStr + std::to_string(tempPort);
	}
	mdlmanServerPort = std::stoi(encodedMsgAsStr.substr(encodedMsgAsStr.length() - 4));
	encodedMsgAsStr.erase(encodedMsgAsStr.length() - 4);

	ans = putMsgTougther(encodedMsgAsStr, msgCode);

	// we connect to client that uses TCP. thats why SOCK_STREAM & IPPROTO_TCP
	_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (_clientSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");

	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(mdlmanServerPort); // port that server will listen to
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = inet_addr(serverIP.c_str());

	// the process will not continue until the server accepts the client
	int status = connect(_clientSocket, (struct sockaddr*)&sa, sizeof(sa));

	if (status == INVALID_SOCKET)
		throw std::exception("Cant connect to client");

	send(_clientSocket, ans.c_str(), ans.size(), 0);  // last parameter: flag. for us will be 0. 

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

std::string vectorToString(std::vector<int> vec) 
{
	std::string str;
	for (auto i = vec.begin(); i != vec.end(); ++i) {
		if (i != vec.begin()) {
			str += ",";
		}
		str += std::to_string(*i);
	}
	return str;
}