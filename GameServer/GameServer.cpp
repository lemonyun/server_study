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
	// 기대하고 있는 함수가 정의된 Allocator를 템플릿 두번째 인자로 지정해 줄 수 있다.

	//vector<Knight, StlAllocator<Knight>> v(100);

	Vector<Knight> v(100);

	map<int32, Knight> m;
	m[100] = Knight();
}
