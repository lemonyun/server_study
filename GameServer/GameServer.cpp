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
#include "ServerPacketHandler.h"
#include <tchar.h>

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


	char sendData[1000] = "가"; // CP949 = KS-X-1001 / KS-X-1003 (로마 1바이트, 한글 2바이트)
	char sendData2[1000] = u8"가"; // UTF-8 = Unicode 문자 집합 사용, (로마 1바이트, 한글 3바이트)
	WCHAR sendData3[1000] = L"가"; // UTF-16 = Unicode (한글/로마 2바이트)
	TCHAR sendData4[1000] = _T("가"); // 프로젝트 속성-> 구성 속성 -> 문자 집합 설정에 따라 달라짐

	while (true)
	{
		vector<BuffData> buffs{ BuffData {100, 1.5f} , BuffData {200, 2.3f}, BuffData{300, 0.7f} };
		SendBufferRef sendBuffer = ServerPacketHandler::Make_S_TEST(1001, 100, 10, buffs, L"안녕하세요");

		GSessionManager->Broadcast(sendBuffer);

		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}