#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"


void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	switch (header.id)
	{
	case S_TEST:
		Handle_S_TEST(buffer, len);
		break;
	}	
}
#pragma pack(1)
// ��Ŷ ���� temp


// ��Ŷ ������Ÿ���� ���� ����ü�� �ƴ� xml�̳� json���� ���� ǥ���Ѵ�.
// Ŭ���̾�Ʈ �ڵ尡 C#�� ��쿡�� vector�� wstring�� ������ ���� ������ ȣȯ�� ����

// ���� ũ�� �����͸� ���� �տ� ���Ƴ��� �������� �����͵��� �ڿ�
// [PKT_S_TEST][BuffsListItem BuffsListItem BuffsListItem]
struct PKT_S_TEST
{
	struct BuffsListItem
	{
		uint64 buffId;
		float remainTime;
	};

	uint16 packetSize;
	uint16 packetId;
	uint64 id;
	uint32 hp;
	uint16 attack;

	// ���� �����Ϳ� ���� ����	
	uint16 buffsOffset; // ���� �����Ͱ� ���۵Ǵ� ������
	uint16 buffsCount; // ���� �������� ����

	bool Validate()
	{
		uint32 size = 0;

		size += sizeof(PKT_S_TEST);
		size += buffsCount * sizeof(BuffsListItem);
		if (size != packetSize)
			return false;

		if (buffsOffset + buffsCount * sizeof(BuffsListItem) > packetSize)
			return false;
	}
	/*vector<BuffData> buffs;
	wstring name;*/
};
#pragma pack()


void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len); 

	if (len < sizeof(PKT_S_TEST))
		return;

	PKT_S_TEST pkt;
	br >> pkt;
	
	if (pkt.Validate() == false)
		return;

	//cout << "ID: " << id << "HP : " << hp << "ATT :" << attack << endl;*/

	vector<PKT_S_TEST::BuffsListItem> buffs;

	buffs.resize(pkt.buffsCount);

	for (int32 i = 0; i < pkt.buffsCount; i++)
		br >> buffs[i];


	cout << "BufCount : " << pkt.buffsCount << endl;

	for (int32 i = 0; i < pkt.buffsCount; i++)
	{
		cout << "Buf Info : " << buffs[i].buffId << " " << buffs[i].remainTime << endl;
	}
}
