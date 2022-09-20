#pragma once
#include "Allocator.h"

template<typename Type, typename ...Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(Xalloc(sizeof(Type)));

	//placement new 문법 (이미 할당된 메모리에서 생성자를 호출하는 방법)
	new(memory) Type(forward<Args>(args)...);
	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	// 메모리를 날리기 전에 소멸자를 호출
	obj->~Type();
	Xrelease(obj);
}


