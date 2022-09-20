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

class Player
{
public:
	Player() {}
	virtual ~Player() {}

};
class Knight : public Player
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}
	Knight(int32 hp) : _hp(hp)
	{
		cout << "Knight(hp)" << endl;
	}

	~Knight()
	{
		cout << "~Knight()" << endl;
	}

	/*void* operator new(size_t size)
	{
		cout << "Knight new! " << size << endl;
		void* ptr = ::malloc(size);
		return ptr;
	}

	void operator delete(void* ptr)
	{
		cout << "Knight delete!" << endl;
		::free(ptr);

	}*/

	int32 _hp = 100;
	int32 _mp = 100;

};

// new operator overloading


int main()
{	
	// 가상 메모리 기본
	// 페이지 단위로 메모리를 할당하며 보안 레벨을 설정할 수 있다.

	//SYSTEM_INFO info;
	//::GetSystemInfo(&info);

	//info.dwPageSize; // 4KB
	//info.dwAllocationGranularity; // 64KB

	//Knight* k1 = new Knight();

	// 위치, 크기, 타입, 페이지 정책을 인자로 받음
	/*int* test = (int*)::VirtualAlloc(NULL, 4, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	*test = 100;

	::VirtualFree(test, 0, MEM_RELEASE);*/

	// *test = 200; // 크래쉬 발생! delete는 메모리 오염을 탐지 못할 수도 있다. delete한다고 해서
	// 메모리를 바로 날리지 않을 수도 있다는 뜻


	//Knight* knight = xnew<Knight>(100);
	//xdelete(knight);

	//knight->_hp = 100;

	// 아래 구문은 오버 플로우 문제가 발생할 수 있는데 StompAllocator에서 못잡아줌
	// 해결 방법 : Alloc에서 데이터 배치 오프셋을 옮긴다.
	Knight* knight1 = (Knight*)xnew<Player>();

	knight1->_hp = 100;
	
	xdelete(knight1);
}
