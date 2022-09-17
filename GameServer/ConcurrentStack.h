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

	// empty �Լ��� �ǹ̰� ����. empty�� ȣ���Ͽ� ������ ������� üũ�Ѵٰ� �ص� ���� �ٸ� �����尡 Push�� ���ɼ��� �ֱ� ������
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

//template<typename T>
//class LockFreeStack
//{
//	struct Node
//	{
//		Node(const T& value) : data(make_shared<T>(value)), next(nullptr)
//		{
//
//		}
//
//		shared_ptr<T> data;
//		shared_ptr<Node> next;
//	};
//
//public:
//	// 1) �� ��带 �����
//	// 2) �� ����� next = head
//	// 3) head = �� ���
//
//	void Push(const T& value)
//	{
//		shared_ptr<Node> node = make_shared<Node>(value);
//		node->next = std::atomic_load(&_head);
//
//		while (std::atomic_compare_exchange_weak(&_head, &node->next, node) == false)
//		{
//
//		}
//
//		//while (_head.compare_exchange_weak(node->next, node) == false)
//		//{
//		//	// node->next = _head
//		//}
//	}
//	// 1) ������ ��带 �д´�.
//	// 2) ����� next�� �д´�.
//	// 3) head = head->next
//	// 4) data �����ؼ� ��ȯ
//	// 5) ������ ��� ����
//
//	shared_ptr<T> TryPop()
//	{
//		shared_ptr<Node> oldHead = std::atomic_load(&_head);
//		while (oldHead && std::atomic_compare_exchange_weak(&_head, &oldHead, oldHead->next) == false)
//		{
//
//		}
//
//		if (oldHead == nullptr)
//			return shared_ptr<T>();
//
//		return oldHead->data;
//
//	}
//
//	// 1) ������ �и�
//	// 2) count üũ
//	// 3) �� ȥ�ڸ� ����
//
//
//private:
//	shared_ptr<Node> _head;
//
//	//atomic<uint32> _popCount = 0; // Pop�� �������� ������ ����
//	//atomic<Node*> _pendingList; // ���� �Ǿ�� �� ���� (ù ��° ���)
//};

template<typename T>
class LockFreeStack
{
	struct Node;

	struct CountedNodePtr
	{
		int32 externalCount = 0;
		Node* ptr = nullptr;
	};

	struct Node
	{
		Node(const T& value) : data(make_shared<T>(value))
		{

		}

		shared_ptr<T> data;
		atomic<int32> internalCount = 0;
		CountedNodePtr next;

	};

public:

	void Push(const T& value)
	{
		CountedNodePtr node;
		node.ptr = new Node(value);
		node.externalCount = 1;
		
		node.ptr->next = _head;
		while (_head.compare_exchange_weak(node.ptr->next, node) == false)
		{

		}
	}


	shared_ptr<T> TryPop()
	{
		CountedNodePtr oldHead = _head;
		while (true)
		{
			// ������ ȹ�� (externalCount�� �� ���� ���� +1 �� �ְ� �̱�)
			IncreaseHeadCount(oldHead);
			// �ּ��� external Count >= 2���״� �������� ����
			Node* ptr = oldHead.ptr;

			if (ptr == nullptr)
				return shared_ptr<T>();

			// ������ ȹ�� (ptr->next�� next�� �ٲ�ġ�� �� �ְ� �̱�)
			if (_head.compare_exchange_strong(oldHead, ptr->next)) 
			{
				shared_ptr<T> res;
				res.swap(ptr->data);

				const int32 countIncrease = oldHead.externalCount - 2;

				if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
					delete ptr;

				return res;
			}
			else if(ptr->internalCount.fetch_sub(1) == 1)
			{
				// �������� ������� �������� ���� ������ �� ��ó��
				delete ptr;
			}
		}

	}

private:
	void IncreaseHeadCount(CountedNodePtr& oldCounter)
	{
		while (true)
		{
			CountedNodePtr newCounter = oldCounter;
			newCounter.externalCount++;

			if (_head.compare_exchange_strong(oldCounter, newCounter))
			{
				oldCounter.externalCount = newCounter.externalCount;
				break;
			}
		}
	}

private:
	atomic<CountedNodePtr> _head;


};
