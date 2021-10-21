//Buffer.cpp
//Gian tullo, 0886424
//290921
//Implements the functionality of a TCP Packet buffer

#include "Buffer.h";

Buffer::Buffer(size_t size) 
{
	buffer = std::vector<uint8_t>(size);
	indexRead = 0;
	indexWrite = 0;
}

//void Buffer::writeUInt32LE(std::size_t index, int32_t value) 
//{
//	buffer[index] = value;
//	buffer[index + 1] = value >> 8;
//	buffer[index + 2] = value >> 16;
//	buffer[index + 3] = value >> 24;
//}

//void Buffer::writeUInt32LE(int32_t value) 
//{
//	//std::size_t index = 0;
//	//for (; index < buffer.size(); index++) {
//	//	buffer[index] = value >> index * 8;
//	//}
//
//	buffer[indexWrite] = value;
//	buffer[indexWrite + 1] = value >> 8;
//	buffer[indexWrite + 2] = value >> 16;
//	buffer[indexWrite + 3] = value >> 24;
//
//	indexWrite += 4;
//}

//uint32_t Buffer::readUInt32LE(std::size_t index) 
//{
//	uint32_t value = buffer[index];
//	value |= buffer[index + 1] << 8;
//	value |= buffer[index + 2] << 16;
//	value |= buffer[index + 3] << 24;
//	return value;
//}

//uint32_t Buffer::readUInt32LE() 
//{
//	//std::size_t index = 0;
//	//uint32_t value = 0;
//	//for (; index < buffer.size(); index++) {
//	//	value  |= buffer[index] << index * 8;
//	//}
//	//return value;
//}

void Buffer::writeUInt32BE(std::size_t index, int32_t value) 
{
	buffer[index] = value >> 24;
	buffer[index + 1] = value >> 16;
	buffer[index + 2] = value >> 8;
	buffer[index + 3] = value;
}

void Buffer::writeUInt32BE(int32_t value) 
{
	buffer[indexWrite] = value >> 24;
	buffer[indexWrite + 1] = value >> 16;
	buffer[indexWrite + 2] = value >> 8;
	buffer[indexWrite + 3] = value;

	indexWrite += 4;
}

uint32_t Buffer::readUInt32BE(std::size_t index) 
{
	uint32_t value = buffer[index] << 24;
	value |= buffer[index + 1] << 16;
	value |= buffer[index + 2] << 8;
	value |= buffer[index + 3];

	return value;
}

uint32_t Buffer::readUInt32BE() 
{
	uint32_t value = buffer[indexRead] << 24;
	value |= buffer[indexRead + 1] << 16;
	value |= buffer[indexRead + 2] << 8;
	value |= buffer[indexRead + 3];

	indexRead += 4;
	return value;
}

void Buffer::writeUInt16BE(std::size_t index, int16_t value)
{
	buffer[index] = value >> 8;
	buffer[index + 1] = value;
}

void Buffer::writeUInt16BE(int16_t value)
{
	buffer[indexWrite] = value >> 8;
	buffer[indexWrite + 1] = value;

	indexWrite += 2;
}

uint16_t Buffer::readUInt16BE(std::size_t index)
{
	uint16_t value = buffer[index] << 8;
	value |= buffer[index + 1];

	return value;
}

uint16_t Buffer::readUInt16BE()
{
	uint16_t value = buffer[indexRead] << 8;
	value |= buffer[indexRead + 1];

	indexRead += 2;
	return value;
}

void Buffer::writeUInt8BE(std::size_t index, std::string value)
{
	int charOrder = 0;
	for (char c : value)
	{
		buffer[index + charOrder] = c;
		charOrder++;
	}
}

void Buffer::writeUInt8BE(std::string value)
{
	int charOrder = 0;
	for (char c : value)
	{
		buffer[indexWrite + charOrder] = c;
		charOrder++;
	}

	indexWrite += charOrder;
}

std::string Buffer::readUInt8BE(std::size_t index, int stringSize)
{
	std::string output;
	for (int i = 0; i < stringSize; i++)
	{
		output.push_back(buffer[index + i]);
	}

	return output;
}

std::string Buffer::readUInt8BE(int stringSize)
{
	std::string output = "";
	for (int i = 0; i < stringSize; i++)
	{
		output.push_back(buffer[indexRead + i]);
		indexRead++;
	}

	return output;
}