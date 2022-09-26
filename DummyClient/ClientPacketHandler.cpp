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
struct PKT_S_TEST
{
	struct BuffsListItem
	{
		uint64 buffId;
		float remainTime;

		uint16 victimsOffset;
		uint16 victimsCount;

		bool Validate(BYTE* packetStart, uint16 packetSize, OUT uint32& size)
		{
			if (victimsOffset + victimsCount * sizeof(uint64) > packetSize)
				return false;

			size += victimsCount * sizeof(uint64);
			return true;
		}
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

		if (packetSize < size)
			return false;

		if (buffsOffset + buffsCount * sizeof(BuffsListItem) > packetSize)
			return false;


		//Buffers의 가변 데이터 크기 추가
		size += buffsCount * sizeof(BuffsListItem);
		
		BuffsList buffList = GetBuffsList();
		for (int32 i = 0; i < buffList.Count(); i++)
		{
			if (buffList[i].Validate((BYTE*)this, packetSize, OUT size) == false)
				return false;
		}

		if (size != packetSize)
			return false;

		return true;
	}

	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;
	using BuffsVictimsList = PacketList<uint64>;

	BuffsList GetBuffsList()
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffsOffset;
		return BuffsList(reinterpret_cast<PKT_S_TEST::BuffsListItem*>(data), buffsCount);
	}

	BuffsVictimsList GetBuffsVictimList(BuffsListItem* buffsItem)
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffsItem->victimsOffset;
		return BuffsVictimsList(reinterpret_cast<uint64*>(data), buffsItem->victimsCount);
	}
	/*vector<BuffData> buffs;
	wstring name;*/
};
#pragma pack()


void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len); 

	// 임시 객체를 사용하지 않는 방법
	PKT_S_TEST* pkt = reinterpret_cast<PKT_S_TEST*>(buffer);

	// 이렇게 복붙할 필요 없다.
	//PKT_S_TEST pkt;
	//br >> pkt;
	
	if (pkt->Validate() == false)
		return;

	//cout << "ID: " << id << "HP : " << hp << "ATT :" << attack << endl;*/

	// 이렇게 복붙할 필요 없다.
	// vector<PKT_S_TEST::BuffsListItem> buffs; 
	//buffs.resize(pkt->buffsCount);

	PKT_S_TEST::BuffsList buffs = pkt->GetBuffsList();

	/*for (int32 i = 0; i < pkt->buffsCount; i++)
		br >> buffs[i];*/


	cout << "BufCount : " << buffs.Count() << endl;

	for (auto& buff : buffs)
	{
		cout << "Buf Info : " << buff.buffId << " " << buff.remainTime << endl;

		PKT_S_TEST::BuffsVictimsList victims = pkt->GetBuffsVictimList(&buff);

		cout << "Victim Count : " << victims.Count() << endl;

		for (auto& victim : victims)
		{
			cout << "Victim : " << victim << endl;
		}
	}

}
