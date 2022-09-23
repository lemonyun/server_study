#pragma once
#include "Allocator.h"

class MemoryPool;

/*---------
   Memory
---------*/
class Memory
{
	enum
	{
		// ~ 1024까지 32단위 ~ 2048까지는 128단위 ~ 4096까지는 256 바이트 단위
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};

public:
	Memory();
	~Memory();

	void* Allocate(int32 size);
	void Release(void* ptr);

private:
	vector<MemoryPool*> _pools;

	// 메모리 크기 <-> 메모리 풀
	// O(1) 메모리 풀을 빠르게 찾기 위한 테이블

	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1];
};
template<typename Type, typename ...Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));

	//placement new 문법 (이미 할당된 메모리에서 생성자를 호출하는 방법)
	new(memory) Type(forward<Args>(args)...);
	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	// 메모리를 날리기 전에 소멸자를 호출
	obj->~Type();
	PoolAllocator::Release(obj);
}

template<typename Type, typename... Args>
shared_ptr<Type> MakeShared(Args&&... args)
{
	return shared_ptr<Type> {xnew<Type>(forward<Args>(args)...), xdelete<Type> };
}

