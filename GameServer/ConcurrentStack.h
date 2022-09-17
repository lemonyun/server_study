#pragma once

#include <mutex>

template<typename T>

class LockStack
{
public:
	LockStack() {}
	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_stack.push(std::move(value));
		_condVar.notify_one();
	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty())
			return false;


		value = std::move(_stack.top());
		_stack.pop();
		return true; 
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] {return _stack.empty() == false;  });
		value = std::move(_stack.top());
		_stack.pop();
	}

	// empty 함수는 의미가 없다. empty를 호출하여 스택이 비었는지 체크한다고 해도 직후 다른 스레드가 Push할 가능성이 있기 때문에
	/*bool Empty()
	{
		lock_guard<mutex> lock(_mutex);
		return _stack.empty();
	}*/

private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _condVar;
};

