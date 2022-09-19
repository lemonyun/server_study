#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	int32 lockId = 0;
	
	auto findit = _nameToId.find(name);
	if (findit == _nameToId.end())
	{
		lockId = static_cast<int32>(_nameToId.size());
		_nameToId[name] = lockId;
		_idToName[lockId] = name;
	}
	else
	{
		lockId = findit->second;
	}

	// 잡고 있는 락이 있었다면
	if (_lockStack.empty() == false)
	{
		// 기존에 발견되지 않은 케이스라면 데드락 여부 다시 확인한다.
		const int32 prevId = _lockStack.top();
		if (lockId != prevId)
		{
			set<int32>& history = _lockHistory[prevId];
			// 새로 추가될 노드와 가장 최신 노드 사이에 간선이 있는지 확인
			if (history.find(lockId) == history.end())
			{	
				// 없다면 간선을 추가하고 사이클 확인
				history.insert(lockId);
				CheckCycle();
			}
		}
	}
	_lockStack.push(lockId);
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);

	if (_lockStack.empty())
		CRASH("MULTIPLE_UNLOCK");

	int32 lockId = _nameToId[name];
	if (_lockStack.top() != lockId)
		CRASH("INVALID_UNLOCK");

	_lockStack.pop();

}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToId.size());
	// 검사 전 초기화
	_discoveredOrder = vector<int32>(lockCount, -1);
	_discoveredCount = 0;
	_finished = vector <bool>(lockCount, false);
	_parent = vector<int32>(lockCount, - 1);

	for (int32 lockId = 0; lockId < lockCount; lockId++)
		Dfs(lockId);

	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::Dfs(int32 here)
{
	if (_discoveredOrder[here] != -1)
		return;

	// 0 1 2 4 3
	// 방문한 노드에 몇번째로 방문했는지를 기록
	// -1이면 아직 방문하지 않은 노드

	_discoveredOrder[here] = _discoveredCount++;

	auto findit = _lockHistory.find(here);

	if (findit == _lockHistory.end())
	{
		_finished[here] = true;
		return;
	}

	set<int32>& nextSet = findit->second;

	for (int32 there : nextSet)
	{
		// 아직 방문한 적이 없다면 방문한다.
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}

		if (_discoveredOrder[here] < _discoveredOrder[there])
			continue; // 순방향 간선의 경우

		if (_finished[there] == false) // 순방향이 아닌데 DFS가 끝나지 않은 상태라면 역방향 간선
		{
			printf("%s -> %s\n", _idToName[here], _idToName[there]);

			int32 now = here;
			while (true)
			{
				printf("%s -> %s\n", _idToName[_parent[now]], _idToName[now]);
				now = _parent[now];
				if (now == there)
					break;
			}

			CRASH("DEADLOCK_DETECTED");
		}
		

	}

	_finished[here] = true;
}
