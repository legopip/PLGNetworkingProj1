//Buffer.h
//Gian tullo, 0886424 / Lucas Magalhaes
//290921
//Outlines the functionality of a TCP Packet buffer

#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

#include "ProtocolTypes.h"

class Buffer {
private:
	int indexRead;
	int indexWrite;
	std::vector<uint8_t> buffer;

public:
	Buffer(std::size_t size);

	void writeUInt32BE(std::size_t index, int32_t value);
	void writeUInt32BE(int32_t value);
	uint32_t readUInt32BE(std::size_t index);
	uint32_t readUInt32BE();

	void writeUInt16BE(std::size_t index, int16_t value);
	void writeUInt16BE(int16_t value);
	uint16_t readUInt16BE(std::size_t index);
	uint16_t readUInt16BE();

	void writeUInt8BE(std::size_t index, std::string value);
	void writeUInt8BE(std::string value);
	std::string readUInt8BE(std::size_t index, int stringSize);
	std::string readUInt8BE(int stringSize);

	std::vector<uint8_t> GetBuffer();
	int GetWriteIndex();

	std::string PayloadToString();

	void ClearBuffer();
};
