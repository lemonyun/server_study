#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int32 bufferSize) :_bufferSize(bufferSize)
{
	_capacity = _bufferSize * BUFFER_COUNT;
	_buffer.resize(_capacity);
}

RecvBuffer::~RecvBuffer()
{

}

void RecvBuffer::Clean()
{
	int32 dataSize = DataSize();
	if (dataSize == 0)
	{
		_readPos = _writePos = 0;
	}
	else
	{
		// 복사를 피하는 방법 : 버퍼 크기를 최대한 키운다.
		if (FreeSize() < _bufferSize)
		{
			::memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
			_readPos = 0;
			_writePos = dataSize;
		}	
	}
}

bool RecvBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;
	
	_readPos += numOfBytes;
	return true;
}

bool RecvBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes > FreeSize())
		return false;

	_writePos += numOfBytes;
	return true;
}
