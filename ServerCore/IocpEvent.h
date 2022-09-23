#pragma once
class Session;

enum class EventType : uint8
{
	Connect,
	Accept,
	Recv,
	Send,
};

/*-----------------
	IocpEvent
-----------------*/

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void Init();
	EventType GetType() { return _type; }

protected:
	EventType _type;

};

/*----------------------
	Connect Event
-------------------*/
class ConnectEvent : public IocpEvent
{
	ConnectEvent() : IocpEvent(EventType::Connect) { }
};

/*----------------------
	Accept Event
-------------------*/
class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}
	void SetSession(Session* session) { _session = session; }
	Session* GetSession() { return _session; }
private:
	Session* _session = nullptr;
	// TODO

};

/*----------------------
	Recv Event
-------------------*/


class RecvEvent : public IocpEvent
{
	RecvEvent() : IocpEvent(EventType::Recv) { }
};

/*----------------------
	Send Event
-------------------*/

class SendEvent : public IocpEvent
{
	SendEvent() : IocpEvent(EventType::Send) { }
};
