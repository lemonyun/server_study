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

	// ��Ŷ ù ��ġ�� ����� ���� ������ ����
	PacketHeader* header = bw.Reserve<PacketHeader>();


	// ������ ���� �κ�
	//id(uint64), ü��(uint32), ���ݷ�(uint16)
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
	
	// ��� ���� �κ�
	header->size = bw.WriteSize();
	header->id = S_TEST; // Hello Msg // �������� ID // ��Ŷ ID


	sendBuffer->Close(bw.WriteSize());
	return sendBuffer;
}
