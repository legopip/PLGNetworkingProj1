#pragma once
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