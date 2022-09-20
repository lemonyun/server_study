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

using KnightRef = TSharedPtr<class Knight>;
using InventoryRef = TSharedPtr<class Inventory>;

class Knight
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}

	~Knight()
	{
		cout << "~Knight()" << endl;
	}

};


int main()
{	
	// 이전 방법(RefCountable 상속하는 방법)의 문제점
	// 1) 이미 만들어진 클래스 대상으로 사용이 불가능하다 
	// 2) 순환 (Cycle) 문제

	//shared_ptr
	//weak_ptr 순환 문제를 해결하기 위함
	// 객체의 클래스 멤버로 다른 객체를 가리킬때 weakptr을 사용하면 해결된다.

	// [Knight | RefCountingBlock(uses, weak)]
	// [T*][RefCountBlock*]

	// RefCountBlock(useCount(shared), weakCount)
	// weakptr은 객체의 수명 주기에는 영향을 주지는 않지만 레퍼런스 블록을 가리켜 객체가 사라졌는지 테스트할 수 있고, 
	// 객체가 존재한다면 참조할 수도 있다.


	shared_ptr<Knight> spr = make_shared<Knight>();
	weak_ptr<Knight> wpr = spr;

	bool expired = wpr.expired();
	shared_ptr<Knight> spr2 = wpr.lock();



	shared_ptr<Knight> spr2 = spr;




	//k2->SetTarget(k1);
}
