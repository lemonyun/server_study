#pragma once

// 서버가 클라에 보내는건 S_ 클라가 서버에 보내는건 C_로 시작
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


