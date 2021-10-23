#pragma once

//ProtocolTypes.h
//Gian tullo, 0886424 / Lucas Magalhaes / Philip 
//231021
//Outlines our protocols

#define DEFAULT_BUFLEN 512

#include <string>

enum ProtocolType
{
	JOIN_ROOM = 1,
	LEAVE_ROOM,
	SEND_MESSAGE,
	RECV_MESSAGE
};


struct sProtocolData
{
	ProtocolType type;
	std::string userName;
	std::string room;
	std::string message;
};

Buffer MakeProtocol(ProtocolType type, std::string name, std::string room, std::string message);



//Retreives data from the buffer into a more usable format
sProtocolData ParseBuffer(Buffer input);
