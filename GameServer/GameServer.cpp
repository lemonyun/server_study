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

class Knight
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
	Knight* knight = xnew<Knight>(100);

	xdelete(knight);

}
