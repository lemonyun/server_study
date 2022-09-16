#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <future>


int64 result;

int64 Calculate()
{
	int64 sum = 0;

	for (int32 i = 0; i < 100000; i++) {
		sum += i;
	}

	return sum;
}

void PromiseWorker(std::promise<string>&& promise)
{
	promise.set_value("Secret Message");
}

void TaskWorker(std::packaged_task<int64(void)> && task)
{
	task();
}

int main() 
{
	// 동기 실행
	// 상대적으로 짧은 작업에 대해서는 쓰레드를 직접 만들고 관리하여 사용하는 방법 대신에 future을 사용하면 좋다.
	// mutex, conditional_variable까지 가지 않고 짧은 일회성 작업에 유용하다.
	int64 sum = Calculate();
	cout << sum << endl;

	/*thread t(Calculate);

	t.join();*/

	{
		// async의 3가지 옵션
		// 1) deferred -> 지연해서 실행하세요 (command 패턴)
		// 2) async -> 별도의 쓰레드를 만들어서 실행하세요 
		// 3) deffered | async -> 둘 중 알아서 골라주세요

		std::future<int64> future = std::async(std::launch::async, Calculate);

		// 딴짓

		std:future_status status = future.wait_for(1ms); 
		if (status == future_status::ready) // 작업의 완료 상태를 확인하는 방법 status가 ready면 완료된 것
		{
			//.. 
		}

		int64 sum = future.get(); // 결과물을 이제는 받고 싶다는 뜻

		// 어떤 객체의 멤버 함수를 future로 호출하고 싶은 경우

		// int64 sum = future.get();

		//class Knight
		//{
		//public:
		//	int64 GetHp() { return 100; }
		//};

		//Knight knight;
		//std::future<int64> future2 = std::async(std::launch::async, &Knight::GetHp, knight); // knight.GetHp(); 

		// future 객체를 만드는 다른 방법

	}

	// future를 사용하는 방법 2번째 : promise
	// std::promise
	// promise와 future는 짝을 맞춰 주어야 한다.
	//{
	//	// 미래(std::future)에 결과물을 반환해줄거라 약속
	//	std::promise<string> promise;
	//	std::future<string> future = promise.get_future();

	//	// promise는 다른 스레드에 넘겨준다.
	//	thread t(PromiseWorker, std::move(promise));

	//	string message = future.get();
	//	cout << message << endl;

	//	t.join(); 

	//}

	// future를 사용하는 방법 3번째 : packaged_task
	// 이미 존재하는 쓰레드에 일감을 넘길 수 있다는 점이 첫번째 방법의 async와 다르다.
	//std::packaged_task
	{
		std::packaged_task<int64(void)> task(Calculate);
		std::future<int64> future = task.get_future();

		std::thread t(TaskWorker, std::move(task));

		int64 sum = future.get();
		cout << sum << endl;

		t.join();
	}
}