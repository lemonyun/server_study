#pragma once

// ������ Ŭ�� �����°� S_ Ŭ�� ������ �����°� C_�� ����
enum
{
	S_TEST = 1 
};

class ClientPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);

	static void Handle_S_TEST(BYTE* buffer, int32 len);
};
