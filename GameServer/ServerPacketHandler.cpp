#include "pch.h"
#include "ServerpacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"

void ServerPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br.Peek(&header);

	switch (header.id)
	{

	default:
		break;
	}
}

SendBufferRef ServerPacketHandler::Make_S_TEST(uint64 id, uint32 hp, uint16 attack, vector<BuffData> buffs, wstring name)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	// 패킷 첫 위치에 헤더를 위한 공간을 예약
	PacketHeader* header = bw.Reserve<PacketHeader>();


	// 데이터 기입 부분
	//id(uint64), 체력(uint32), 공격력(uint16)
	//bw << (uint64)1001 << (uint32)100 << (uint16)10;
	bw << id << hp << attack;

	struct ListHeader
	{
		uint16 offset;
		uint16 count;
	};

	ListHeader* buffsHeader = bw.Reserve<ListHeader>();
	
	buffsHeader->offset = bw.WriteSize(); 
	buffsHeader->count = buffs.size();

	for (BuffData& buff : buffs)
		bw << buff.buffId << buff.remainTime;
	
	// 헤더 기입 부분
	header->size = bw.WriteSize();
	header->id = S_TEST; // Hello Msg // 프로토콜 ID // 패킷 ID


	sendBuffer->Close(bw.WriteSize());
	return sendBuffer;
}
