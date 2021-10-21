//Buffer.cpp
//Gian tullo, 0886424
//290921
//Implements the functionality of a TCP Packet buffer

#include "Buffer.h";

Buffer::Buffer(size_t size) {
	buffer = std::vector<uint8_t>(size);
}

void Buffer::writeUInt32LE(std::size_t index, int32_t value) {
	buffer[index] = value;
	buffer[index + 1] = value >> 8;
	buffer[index + 2] = value >> 16;
	buffer[index + 3] = value >> 24;
}

void Buffer::writeUInt32LE(int32_t value) {
	std::size_t index = 0;
	for (; index < buffer.size(); index++) {
		buffer[index] = value >> index * 8;
	}
}

uint32_t Buffer::readUInt32LE(std::size_t index) {
	uint32_t value = buffer[index];
	value |= buffer[index + 1] << 8;
	value |= buffer[index + 2] << 16;
	value |= buffer[index + 3] << 24;
	return value;
}

uint32_t Buffer::readUInt32LE() {
	std::size_t index = 0;
	uint32_t value = 0;
	for (; index < buffer.size(); index++) {
		value  |= buffer[index] << index * 8;
	}
	return value;
}