#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>

#include <windows.h>

mutex m;
queue<int32> q;
HANDLE handle;

void Producer()
{
	while (true)
	{
		{
			unique_lock<mutex> lock(m);
			q.push(100);
		}

		::SetEvent(handle); // 핸들의 상태를 Signal 상태로 바꿈
		this_thread::sleep_for(100ms);
	}
}

void Consumer()
{
	while (true)
	{
		::WaitForSingleObject(handle, INFINITE);
		// auto 이벤트의 경우 깨자마자 Signal이 non signal 상태로 자동으로 바뀜
		// manual 이벤트의 경우 ::ResetEvent(handle)을 직접 호출해줘야 non signal 상태로 바뀜
		unique_lock<mutex> lock(m);
		if (q.empty() == false)
		{
			int32 data = q.front();
			q.pop();
			cout << data << endl;
		}
	}
}

int main()
{
	// 커널을 사용하기 때문에 너무 빈번하게 이벤트를 사용하면 오히려 안 좋을 수도 있다.
	// 커널 오브젝트
	// Usage Count
	// Signal / Non-Signal (bool)
	// Auto / Manual
	handle = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	thread t1(Producer);
	thread t2(Consumer);
	
	t1.join();
	t2.join();

	::CloseHandle(handle);
}