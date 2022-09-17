#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>

#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"

// queue와 lock을 가지는 자료구조를 정의하기
// 네트워크 게임에서는 큐를 사용한다 클라이언트 쪽에서 보낸 패킷을 순차적으로 서버에서 처리하기 위해 큐를 사용한다.

LockQueue<int32> q;
LockFreeStack<int32> s;

void Push()
{
	while (true)
	{
		int32 value = rand() % 100;
		s.Push(value);

		this_thread::sleep_for(10ms);
	}
}

void Pop()
{
	while (true)
	{
		auto data = s.TryPop();
		if (data != nullptr)
			cout << (*data) << endl;

	}
}

int main()
{
	thread t1(Push);
	thread t2(Pop);
	thread t3(Pop);

	t1.join();
	t2.join();
	t3.join();
}
