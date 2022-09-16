#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>

atomic<bool> ready;
int32 value;

void Producer()
{
	value = 10;
	ready.store(true, memory_order_seq_cst);
}

void Consumer()
{
	while (ready.load(memory_order::memory_order_seq_cst) == false)
		;
	cout << value << endl;
}

int main()
{
	ready = false;
	value = 0;
	
	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	// Memory Model (정책)
	// 1) Sequentially Consistent (seq_cst)
	// ㄴ 가시성 문제 + 코드 재배치 문제 바로 해결
	// 
	// 2) Acquire-Release (consume, acquire, release, acq_rel)
	// release 명령 이전의 메모리 명령들이 해당 명령 이후로 재배치 되는 것을 금지
	// 그리고 acquire로 같은 변수를 읽는 쓰레드가 있다면 
	// release 이전의 명령들이 acquire하는 순간에 관찰 가능 (가시성 보장)
	// 
	// 3) Relaxed (relaxed)
	// 자율성 100%, 동일 객체에 대한 동일 관전 순서만 보장
	// 거의 사용할 일이 없다.

	// 인텔 AMD의 경우 애초에 순차적 일관성을 보장하기 때문에
	// seq_cst를 써도 별다른 부하가 없음
	// ARM의 경우 꽤 차이가 있다.
	
	//atomic<bool> flag = false;

	//flag.is_lock_free();

	//// 이전 flag 값을 prev에 넣고, flag 값을 수정
	//{
	//	bool prev = flag.exchange(true);
	//	// bool prev = flag;
	//	// flag = true;
	//}

	//// CAS (Compare-And-Swap) 조건부 수정
	//{
	//	bool expected = false;
	//	bool desired = true;
	//	flag.compare_exchange_strong(expected, desired);
	//}

	//// flag.compare_exchange_weak
	//// 비교 값이 같아서 true를 반환해야 하는데 어떤 상황에 의해 false를 반환할 수 있다.
	//// flag.compare_exchange_strong
	//// 정확한 값을 반환할 때까지 루프를 돈다.(부하가 있음)
}


// std::atomic_thread_fence(memory_order::memory_order_release)
// 아토믹 객체를 사용하지 않고도 메모리 가시성 강제 + 메모리 재배치 금지하는 방법
// 실전에서 사용할 일은 없다
