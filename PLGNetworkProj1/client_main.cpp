#define WIN32_LEAN_AND_MEAN

//client_main.cpp
//Gian tullo, 0886424 / Lucas Magalhaes / Philip
//231021
//A chatroom client, allowing for the sending and receiving of messages

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

std::vector<std::string> rooms;

//Assembles a Protocol for the buffer
Buffer MakeProtocol(ProtocolType type, std::string name, std::string room, std::string message)
{
	Buffer tempBuf(DEFAULT_BUFLEN);

	if (type == JOIN_ROOM)
	{
		tempBuf.writeUInt32BE(name.length());
		tempBuf.writeUInt8BE(name);

		tempBuf.writeUInt32BE(room.length());
		tempBuf.writeUInt8BE(room);

		//length of everything, including header, which is 8 bytes (2 ints)
		int length = tempBuf.GetWriteIndex() + 8;
		tempBuf.writeUInt32BE(0, length);

		tempBuf.writeUInt32BE(4, JOIN_ROOM);

		rooms.push_back(room);
	}
	else if (type == LEAVE_ROOM)
	{
		tempBuf.writeUInt32BE(name.length());
		tempBuf.writeUInt8BE(name);

		tempBuf.writeUInt32BE(room.length());
		tempBuf.writeUInt8BE(room);

		//length of everything, including header, which is 8 bytes (2 ints)
		int length = tempBuf.GetWriteIndex() + 8;
		tempBuf.writeUInt32BE(0, length);

		tempBuf.writeUInt32BE(4, LEAVE_ROOM);

		rooms.push_back(room);
	}
	else if (type == SEND_MESSAGE)
	{
		tempBuf.writeUInt32BE(name.length());
		tempBuf.writeUInt8BE(name);

		tempBuf.writeUInt32BE(room.length());
		tempBuf.writeUInt8BE(room);

		tempBuf.writeUInt32BE(message.length());
		tempBuf.writeUInt8BE(message);

		int length = tempBuf.GetWriteIndex() + 8;
		tempBuf.writeUInt32BE(0, length);

		tempBuf.writeUInt32BE(4, SEND_MESSAGE);
	}
	else if (type == RECV_MESSAGE) // probably going to delete
	{
		tempBuf.writeUInt32BE(name.length());
		tempBuf.writeUInt8BE(name);

		tempBuf.writeUInt32BE(room.length());
		tempBuf.writeUInt8BE(room);

		tempBuf.writeUInt32BE(message.length());
		tempBuf.writeUInt8BE(message);

		int length = tempBuf.GetWriteIndex() + 8;
		tempBuf.writeUInt32BE(0, length);

		tempBuf.writeUInt32BE(4, RECV_MESSAGE);
	}

	return tempBuf;
}


//Retreives data from the buffer into a more usable format
sProtocolData ParseBuffer(Buffer input)
{
	sProtocolData data;
	int length;
	length = input.readUInt32BE();
	data.type = (ProtocolType)input.readUInt32BE();
	
	if (data.type == JOIN_ROOM)
	{
		int itemLength = input.readUInt32BE();
		data.userName = input.readUInt8BE(itemLength);

		itemLength = input.readUInt32BE();
		data.room = input.readUInt8BE(itemLength);

		// not sure if this is the right way to do it
		// or if we are doing this on the loop
		data.message = data.userName + " entered " + data.room + " room";
	}
	else if (data.type == LEAVE_ROOM)
	{
		int itemLength = input.readUInt32BE();
		data.userName = input.readUInt8BE(itemLength);

		itemLength = input.readUInt32BE();
		data.room = input.readUInt8BE(itemLength);

		data.message = data.userName + " left the " + data.room + " room";
	}
	else if (data.type == SEND_MESSAGE) // not going to be used
	{

	}
	else if (data.type == RECV_MESSAGE)
	{

		uint32_t nameLength = incoming.readUInt32BE();
		data.userName = incoming.readUInt8BE(nameLength);
		uint32_t roomLength = incoming.readUInt32BE();
		data.room = incoming.readUInt8BE(roomLength);
		uint32_t msgLength = incoming.readUInt32BE();
		data.message = incoming.readUInt8BE(msgLength);
	}

	return data;
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

	//Get login info from user
	//this is blocking, but that okay since we're no in a room yet
	std::string username = "";
	std::string roomname = "";
	std::cout << "Enter your username:" << std::endl;
	std::cin >> username;
	std::cout << "Enter the name of the room you want to join:" << std::endl;
	std::cin >> roomname;

	//assemble the protocol
	outgoing = MakeProtocol(JOIN_ROOM, username, roomname, "");//this has no inherent message
	sProtocolData data = ParseBuffer(outgoing);

	//change it to a format we can transport
	char* payload = outgoing.PayloadToString();
	//send it
	result = send(connectSocket, payload, outgoing.readUInt32BE(0), 0);
	if (result == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	//clean up
	delete[] payload;
	printf("Bytes Sent: %ld\n", result);
	
	//start Client loop
	bool updateLog = false;
	bool quit = false;
	while (!quit) {

		//get keyboard input
		if (_kbhit()) {
			char key = _getch();

			if (key == 27) { //esc to quit
				quit = true;
			}else if (key == 8) { //back to remove
				message.pop_back();
				updateLog = true;
			}
			else if (key == 13) { // enter to send

				//call to asseble the protocol
				outgoing = MakeProtocol(SEND_MESSAGE, username, roomname, message);
				sProtocolData data = ParseBuffer(outgoing);

				//change it to a form we can transport
				char* payload = outgoing.PayloadToString();
				//send it
				result = send(connectSocket, payload, outgoing.readUInt32BE(0), 0);
				if (result == SOCKET_ERROR)
				{
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(connectSocket);
					WSACleanup();
					return 1;
				}

				//clean up
				delete[] payload;
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
				
				//get the inbound message, and put it in the buffer
				printf("Bytes received: %d\n", result);
				std::string received = "";
				for (int i = 0; i < recvbuflen; i++) {
					received.push_back(recvbuf[i]);
				}
				incoming.LoadBuffer(received);

				//Parse the data in the buffer

				sProtocolData data = ParseBuffer(incoming);

				//if it comes from a room that we're in, add it to the chat log
				for (int i = 0; i < rooms.size(); i++) {
					system("cls"); //supposedly this isn't a safe thing to do, but I'm pretty sure LG showed it in class
					if (rooms[i] == data.room) { 
						chatlog.push_back(data.userName + ":\t" + data.message); 
					}
					updateLog = true;
				}
			}
			else if (result == 0)
			{
				//if we've left
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
				//user input
				std::cout << "message: ";
				std::cout << message << std::endl;
				updateLog = false;
			}

		}

	}

	//Leave
	outgoing = MakeProtocol(LEAVE_ROOM, username, roomname, "");//this has no inherent message
	sProtocolData leaveData = ParseBuffer(outgoing);

	//change it to a format we can transport
	char* leavePayload = outgoing.PayloadToString();
	//send it
	result = send(connectSocket, leavePayload, outgoing.readUInt32BE(0), 0);
	if (result == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	//clean up
	delete[] leavePayload;
	printf("Bytes Sent: %ld\n", result);

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