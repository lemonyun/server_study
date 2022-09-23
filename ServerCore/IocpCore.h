#pragma once
// iocp는 두가지 방식으로 데이터를 넘길 수 있다.
// key값으로 넣어주는 데이터 -> iocpObject
// overlapped 구조체를 통해 얻는 데이터 -> iocpEvent

// complete port에는 network 관련 작업 뿐만 아니라 다른 것도 넣어줄 수 있기 때문에
// 그런 것들을 활용하기 위해 cp에 넣는 것들은 iocpobject로 통일하고 이것을 상속 받도록

class IocpObject // session의 역할
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE GetHandle() { return _iocpHandle; }
	bool Register(class IocpObject* iocpObject); // 소켓 cp에 등록하는 함수
	bool Dispatch(uint32 timeoutMs = INFINITE); // worker 쓰레드 들이 일감이 있는지 확인하는 함수


private:
	HANDLE _iocpHandle;
};

extern IocpCore GIocpCore;

