#pragma once
// iocp�� �ΰ��� ������� �����͸� �ѱ� �� �ִ�.
// key������ �־��ִ� ������ -> iocpObject
// overlapped ����ü�� ���� ��� ������ -> iocpEvent

// complete port���� network ���� �۾� �Ӹ� �ƴ϶� �ٸ� �͵� �־��� �� �ֱ� ������
// �׷� �͵��� Ȱ���ϱ� ���� cp�� �ִ� �͵��� iocpobject�� �����ϰ� �̰��� ��� �޵���

class IocpObject // session�� ����
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
	bool Register(class IocpObject* iocpObject); // ���� cp�� ����ϴ� �Լ�
	bool Dispatch(uint32 timeoutMs = INFINITE); // worker ������ ���� �ϰ��� �ִ��� Ȯ���ϴ� �Լ�


private:
	HANDLE _iocpHandle;
};

extern IocpCore GIocpCore;

