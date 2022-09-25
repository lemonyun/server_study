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
// 패킷 설계 temp


// 패킷 프로토타입은 보통 구조체가 아닌 xml이나 json으로 보통 표현한다.
// 클라이언트 코드가 C#인 경우에는 vector나 wstring의 개념이 없기 때문에 호환을 위해

// 고정 크기 데이터를 먼저 앞에 몰아놓고 가변적인 데이터들은 뒤에
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

	// 가변 데이터에 대한 정보	
	uint16 buffsOffset; // 가변 데이터가 시작되는 오프셋
	uint16 buffsCount; // 가변 데이터의 개수

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
