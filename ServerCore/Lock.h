#pragma once

#include "Types.h"
/*-------------
* RW SpinLock
---------------*/

/*---------------
[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
W : WriteFlag(Exclusive Lock Owner ThreadId)
R : ReadFlag (Shared Lcok Count)
----------------*/

// Write Lock을 잡은 상태에서 Read Lock을 잡을 수는 있지만
// Read Lock을 잡은 상태에서 Write Lock을 잡을 수는 없다.
class Lock
{

	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0xFFFF0000,
		READ_COUNT_MASK = 0x0000FFFF,
		EMPTY_FLAG = 0x00000000,
	};

public:
	void WriteLock();
	void WriteUnlock();

	void ReadLock();
	void ReadUnlock();

private:
	Atomic<uint32> _lockFlag = EMPTY_FLAG;
	uint16 _writeCount = 0;

};

/*------------------
	LockGuards
-------------------*/

class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock) : _lock(lock)
	{
		_lock.ReadLock();
	}
	~ReadLockGuard()
	{
		_lock.ReadUnlock();
	}

private:
	Lock& _lock;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock) : _lock(lock)
	{
		_lock.WriteLock();
	}
	~WriteLockGuard()
	{
		_lock.WriteUnlock();
	}

private:
	Lock& _lock;
};