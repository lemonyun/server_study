#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"
#include "RefCounting.h"
#include "Memory.h"
#include "Allocator.h"

// LockFreeStack을 사용할 일이 있으면 마이크로소프트에서 제공하는 함수를 사용하자.

class Knight
{
public:
	int32 _hp = rand() % 1000;
};

int main()
{	
	

	for (int32 i = 0; i < 5; i++) {
		GThreadManager->Launch([]() {
			while (true)
			{
				Knight* knight = xnew<Knight>();

				cout << knight->_hp << endl;
				this_thread::sleep_for(10ms);

				xdelete(knight);
			}
		});
	}

	GThreadManager->Join();
}
