#pragma once

//ProtocolHelper.h
//Gian tullo, 0886424 / Lucas Magalhaes / Philip 
//231021
//Outlines our protocols

#define DEFAULT_BUFLEN 512

#include <string>
#include "Buffer.h"

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


class ProtocolMethods
{
	ProtocolMethods() = delete;

public:
	//assembles a buffer using the protocal data
	static Buffer MakeProtocol(ProtocolType type, std::string name, std::string room, std::string message);

	//Retreives data from the buffer into a more usable format
	static sProtocolData ParseBuffer(Buffer input);
};




