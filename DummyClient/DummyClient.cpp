#include "pch.h"
#include <iostream>
#include "ThreadManager.h"

#include "Service.h"
#include "Session.h"
#include "BufferReader.h"

char sendData[] = "Hello Wrold";

class ServerSession : public PacketSession
{
public:
	~ServerSession()
	{
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override
	{
		//cout << "Connected To Server" << endl;

	}

	virtual int32 OnRecvPacket(BYTE* buffer, int32 len) override
	{
		BufferReader br(buffer, len);
		
		PacketHeader header;
		// header에 buffer의 헤더 부분 추출, _pos가 헤더 크기만큼 이동
		br >> header;

		uint64 id;
		uint32 hp;
		uint16 attack;

		// 보내는 쪽에서의 데이터 보내는 순서와 맞춰줘야 한다.
		br >> id >> hp >> attack;

		cout << "ID: " << id << "HP : " << hp << "ATT :" << attack << endl;

		char recvBuffer[4096];
		// 원래 가변 길이 데이터(sendData) 보낼때 몇개 있는지 보내줘야하는데 
		br.Read(recvBuffer, header.size - sizeof(PacketHeader) - 8 - 4 - 2);
		cout << recvBuffer << endl;

		return len;
	}

	virtual void OnSend(int32 len) override {
		//cout << "OnSend Len = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		//cout << "Disconnected" << endl;
	}
};


int main()
{
	this_thread::sleep_for(1s);

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>,
		1000);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

}