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

// ��Ŷ ���� temp
struct BuffData
{
	uint64 buffId;
	float remainTime;
};
struct S_TEST
{
	uint64 id;
	uint32 hp;
	uint16 attack;
	// ���� ������
	// 1) ���ڿ� (ex. name)
	// 2) ����Ʈ �迭 (ex. ��� �̹���)
	// 3) �Ϲ� ����Ʈ
	vector<BuffData> buffs;

	wstring name;
};

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;
	
	uint64 id;
	uint32 hp;
	uint16 attack;

	// ������ �ʿ����� ������ ������ ������ ������� �Ѵ�.
	br >> id >> hp >> attack;

	cout << "ID: " << id << "HP : " << hp << "ATT :" << attack << endl;

	vector<BuffData> buffs;
	uint16 buffCount;
	br >> buffCount;

	buffs.resize(buffCount);

	for (int32 i = 0; i < buffCount; i++)
	{
		br >> buffs[i].buffId >> buffs[i].remainTime;
	}

	cout << "BufCount : " << buffCount << endl;

	for (int32 i = 0; i < buffCount; i++)
	{
		cout << "BuffInfo : " << buffs[i].buffId << " " << buffs[i].remainTime << endl;
	}

	wstring name;
	uint16 nameLen;

	br >> nameLen;
	name.resize(nameLen);

	br.Read((void*)name.data(), nameLen * sizeof(WCHAR));

	wcout.imbue(std::locale("kor"));
	wcout << name << endl;
}