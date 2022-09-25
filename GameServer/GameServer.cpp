#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

#include "Service.h"
#include "Session.h"

#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"

int main()
{
	GSessionManager = new GameSessionManager();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}



	char sendData[1000] = "Hello Wrold";

	while (true)
	{
		SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

		BufferWriter bw(sendBuffer->Buffer(), 4096);

		// 패킷 첫 위치에 헤더를 위한 공간을 예약
		PacketHeader* header = bw.Reserve<PacketHeader>();


		// 데이터 기입 부분
		//id(uint64), 체력(uint32), 공격력(uint16)
		bw << (uint64)1001 << (uint32)100 << (uint16)10;
		// 가변길이 데이터 기입
		bw.Write(sendData, sizeof(sendData));

		// 헤더 기입 부분
		header->size = bw.WriteSize();
		header->id = 1; // Hello Msg // 프로토콜 ID // 패킷 ID


		sendBuffer->Close(bw.WriteSize());

		GSessionManager->Broadcast(sendBuffer);

		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}