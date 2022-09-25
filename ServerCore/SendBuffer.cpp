#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32 allocSize) : _owner(owner), _buffer(buffer), _allocSize(allocSize)
{

}

SendBuffer::~SendBuffer()
{

}

void SendBuffer::Close(uint32 writeSize)
{
	ASSERT_CRASH(_allocSize >= writeSize);
	_writeSize = writeSize;
	_owner->Close(writeSize);

}

SendBufferChunk::SendBufferChunk()
{
}

SendBufferChunk::~SendBufferChunk()
{
}

void SendBufferChunk::Reset()
{
	_open = false;
	_usedSize = 0;
}

SendBufferRef SendBufferChunk::Open(uint32 allocSize)
{
	ASSERT_CRASH(allocSize <= SEND_BUFFER_CHUNK_SIZE);
	ASSERT_CRASH(_open == false);

	if (allocSize > FreeSize())
		return nullptr;

	_open = true;
	return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), Buffer(), allocSize);

}

void SendBufferChunk::Close(uint32 writeSize)
{
	ASSERT_CRASH(_open == true);
	_open = false;
	_usedSize += writeSize;


}


// 스레드별로 자신만의 send buffer를 보유 하도록 tls를 이용
SendBufferRef SendBufferManager::Open(uint32 size)
{
	// TLS 영역을 최대한 활용하여 LOCK을 거는 일을 줄인다.
	if (LSendBufferChunk == nullptr) {
		LSendBufferChunk = Pop();
		LSendBufferChunk->Reset();
	}

	ASSERT_CRASH(LSendBufferChunk->IsOpen() == false);

	// 다 썼으면 버리고 새거로 교체

	if (LSendBufferChunk->FreeSize() < size)
	{
		LSendBufferChunk = Pop(); // WRITE_LOCK
		LSendBufferChunk->Reset();
	}
	
	cout << "FREE : " << LSendBufferChunk->FreeSize() << endl;

	return LSendBufferChunk->Open(size);

}

SendBufferChunkRef SendBufferManager::Pop()
{
	cout << "PopGlobal SENDBUFFERCHUNK" << endl;
	{
		WRITE_LOCK;
		if (_sendBufferChunks.empty() == false)
		{
			SendBufferChunkRef sendBufferChunk = _sendBufferChunks.back();
			_sendBufferChunks.pop_back();
			return sendBufferChunk;
		}
	}
	return SendBufferChunkRef(xnew<SendBufferChunk>(), PushGlobal);
}

void SendBufferManager::Push(SendBufferChunkRef buffer)
{
	WRITE_LOCK;
	_sendBufferChunks.push_back(buffer);
}

void SendBufferManager::PushGlobal(SendBufferChunk* buffer)
{
	cout << "Push Global SENDBUFFERCHUNK" << endl;
	GSendBufferManager->Push(SendBufferChunkRef(buffer, PushGlobal));

}


