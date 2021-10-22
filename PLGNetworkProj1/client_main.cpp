#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <Buffer.h>
#include <ProtocolTypes.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512						// Default buffer length of our buffer in characters
#define DEFAULT_PORT "27015"					// The default port to use
#define SERVER "127.0.0.1"						// The IP of our server

Buffer outgoing(DEFAULT_BUFLEN);
Buffer incoming(DEFAULT_BUFLEN);


Buffer MakeProtocol(ProtocolType type, std::string name, std::string room, std::string message)
{
	Buffer tempBuf(DEFAULT_BUFLEN);

	if (type == JOIN_ROOM)
	{
		tempBuf.writeUInt32BE(name.length());
		tempBuf.writeUInt8BE(name);

		tempBuf.writeUInt32BE(room.length());
		tempBuf.writeUInt8BE(room);
	}

	//length of everything, including header, which is 8 bytes (2 ints)
	int length = tempBuf.GetWriteIndex() + 8;



}



int main(int argc, char **argv)
{
	WSADATA wsaData;							// holds Winsock data
	SOCKET connectSocket = INVALID_SOCKET;		// Our connection socket used to connect to the server

	struct addrinfo *infoResult = NULL;			// Holds the address information of our server
	struct addrinfo *ptr = NULL;
	struct addrinfo hints;
	u_long mode = 1;

	//const char *sendbuf = "Hello World!";		// The messsage to send to the server
	std::string message = "";
	std::vector<std::string> chatlog;

	char recvbuf[DEFAULT_BUFLEN];				// The maximum buffer size of a message to send
	int result;									// code of the result of any command we use
	int recvbuflen = DEFAULT_BUFLEN;			// The length of the buffer we receive from the server

	// Step #1 Initialize Winsock
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		printf("WSAStartup failed with error: %d\n", result);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Step #2 Resolve the server address and port
	result = getaddrinfo(SERVER, DEFAULT_PORT, &hints, &infoResult);
	if (result != 0)
	{
		printf("getaddrinfo failed with error: %d\n", result);
		WSACleanup();
		return 1;
	}

	bool isConnected = false;
	// Step #3 Attempt to connect to an address until one succeeds
	for (ptr = infoResult; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (connectSocket == INVALID_SOCKET)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}


		// Connect to server.
		result = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (result == SOCKET_ERROR)
		{
			closesocket(connectSocket);
			connectSocket = INVALID_SOCKET;
			continue;
		}

		result = ioctlsocket(connectSocket, FIONBIO, &mode);
		if (result != NO_ERROR) {
			printf("ioctlsocket failed with error: %ld\n", result);
			
		}
			isConnected = true;
		break;
	}

	freeaddrinfo(infoResult);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	//Right now I'm assuming that this can be blocking
	std::string username = "";
	std::string roomname = "";
	std::cout << "Enter your username:" << std::endl;
	std::cin >> username;
	std::cout << "Enter the name of the room you want to join:" << std::endl;
	std::cin >> roomname;
	//TODO: form a Login Call with the data
	
	
	bool updateLog = false;
	bool quit = false;
	while (!quit) {

		if (_kbhit()) {
			char key = _getch();

			if (key == 27) { //esc to quit
				quit = true;
			}else if (key == 8) { //back to remove
				message.pop_back();
			}
			else if (key == 13) { // enter to send
				//the code here doesn't work, it return error (-1)
				//error code 10057 (socket not connected!)
				result = send(connectSocket, message.c_str(), (int)strlen(message.c_str()), 0);
				if (result == SOCKET_ERROR)
				{
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(connectSocket);
				WSACleanup();
				return 1;
				}
				printf("Bytes Sent: %ld\n", result);
				message = "";
			}
			else {
				message.push_back(key);
				system("cls"); //supposedly this isn't a safe thing to do, but I'm pretty sure LG showed it in class
				updateLog = true;
				
			}
		}

		if (isConnected) {
			result = recv(connectSocket, recvbuf, recvbuflen, 0);
			if (result > 0)
			{
				printf("Bytes received: %d\n", result);
				//printf("Message: %s\n", &recvbuf);
				system("cls"); //supposedly this isn't a safe thing to do, but I'm pretty sure LG showed it in class
				chatlog.push_back(recvbuf);
				updateLog = true;
			}
			else if (result == 0)
			{
				printf("Connection closed\n");
			}
			else
			{
				if (WSAGetLastError() == 10035) {
					//this error isn't actually bad, it just means we've not received anything
				}
				else {
					printf("Message error happend, opps. time to exit");
					quit = true;
				}
			}

			//print out the chat log, if it need to be updated
			if (updateLog) {
				for (int i = 0; i < chatlog.size(); i++) {
					std::cout << chatlog[i] << std::endl;
				}
				std::cout << std::endl;
				std::cout << "message: ";
				std::cout << message << std::endl;
				updateLog = false;
			}

		}

	}

	// Step #5 shutdown the connection since no more data will be sent
	result = shutdown(connectSocket, SD_SEND);
	if (result == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	// Step #7 cleanup
	closesocket(connectSocket);
	WSACleanup();

	system("pause");

	return 0;
}