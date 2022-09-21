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

using TL = TypeList<class Player, class Mage, class Knight, class Archer>;

// LockFreeStack을 사용할 일이 있으면 마이크로소프트에서 제공하는 함수를 사용하자.
class Player
{
	
public:

	Player()
	{
		INIT_TL(Player);
	}

	virtual ~Player() { }
	
	DECLARE_TL
};

class Knight : public Player
{

public:
	Knight() { INIT_TL(Knight); }
};

class Mage : public Player
{
public:
	Mage() { INIT_TL(Mage);
	}
};

class Archer : public Player
{
public:
	Archer() { INIT_TL(Archer); }
};

class Dog {

};



int main()
{	
	/*TypeList<Mage, Knight>::Head whoAMI;
	TypeList<Mage, Knight>::Tail whoAMI2;

	TypeList<Mage, TypeList<Knight, Archer>>::Head whoAMI3;
	TypeList<Mage, TypeList<Knight, Archer>>::Tail::Head whoAMI4;
	TypeList<Mage, Knight, Archer>::Tail::Tail whoAMI5;

	int32 len1 = Length<TypeList<Mage, Knight>>::value;
	int32 len2 = Length<TypeList<Mage, Knight, Archer>>::value;

	using TL = TypeList<Player, Mage, Knight, Archer>;
	TypeAt<TL, 0>::Result whoAMI6;
	TypeAt<TL, 1>::Result whoAMI7;
	TypeAt<TL, 2>::Result whoAMI8;
	
	int index1 = IndexOf<TL, Mage>::value;
	int index2 = IndexOf<TL, Archer>::value;
	int index3 = IndexOf<TL, Dog>::value;

	bool canConvert1 = Conversion<Player, Knight>::exists;
	bool canConvert2 = Conversion<Knight, Player>::exists;
	bool canConvert3 = Conversion<Knight, Dog>::exists;*/
	
	// dynamic cast는 형변환이 불가능하면 nullptr을 반환
	// dynamic cast는 너무 느려서 성능상의 이유로 static_cast를 씀
	
	// 언리얼 엔진의 경우 dynamic_cast를 직접적으로 활용하지 않고 내부적으로 지원해주는 cast를 사용한다.


	/*{
		Player* player = new Knight();

		bool canCast = CanCast<Knight*>(player);
		Knight* knight = TypeCast<Knight*>(player);

		delete player;
	}*/

	{
		shared_ptr<Player> player = MakeShared<Knight>();

		shared_ptr<Archer> archer = TypeCast<Archer>(player);

		bool canCast = CanCast<Mage>(player);
	}

	for (int32 i = 0; i < 5; i++) {
		GThreadManager->Launch([]() {
			while (true)
			{
				
			}
		});
	}	

	GThreadManager->Join();
}
