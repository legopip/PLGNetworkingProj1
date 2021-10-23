#pragma once

//Buffer.h
//Gian tullo, 0886424 / Lucas Magalhaes / Philip 
//231021
//Outlines the functionality of a TCP Packet buffer

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