//Buffer.h
//Gian tullo, 0886424
//290921
//Outlines the functionality of a TCP Packet buffer

#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>

//size_t size
class Buffer {
public:
	std::vector<uint8_t> buffer;

	Buffer(std::size_t size);
	void writeUInt32LE(std::size_t index, int32_t value);
	void writeUInt32LE(int32_t value);
	uint32_t readUInt32LE(std::size_t index);
	uint32_t readUInt32LE();
};
