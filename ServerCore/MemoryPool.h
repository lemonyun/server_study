#pragma once

enum
{
	SLIST_ALIGNMENT = 16
};

// 다양한 크기의 메모리 풀
// 1. [32] [64] [ ] [ ] 
// 2. [32] [32] [32] [32] [32]


/*--------------------
		MemoryHeaer
---------------------*/
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : SLIST_ENTRY
{
	MemoryHeader(int32 size) : allocSize(size) {}

	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size);
		return reinterpret_cast<void*>(++header);
	}

	static MemoryHeader* DetachHeader(void* ptr) {
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 allocSize;
};


/*--------------
	MemoryPool
--------------*/
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void Push(MemoryHeader* ptr);
	MemoryHeader* Pop();


private:
	SLIST_HEADER _header;
	int32 _allocSize = 0;
	atomic<int32> _useCount = 0;
	atomic<int32> _rserveCount = 0;

};

