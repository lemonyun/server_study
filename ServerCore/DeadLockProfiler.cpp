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

	// ��� �ִ� ���� �־��ٸ�
	if (_lockStack.empty() == false)
	{
		// ������ �߰ߵ��� ���� ���̽���� ����� ���� �ٽ� Ȯ���Ѵ�.
		const int32 prevId = _lockStack.top();
		if (lockId != prevId)
		{
			set<int32>& history = _lockHistory[prevId];
			// ���� �߰��� ���� ���� �ֽ� ��� ���̿� ������ �ִ��� Ȯ��
			if (history.find(lockId) == history.end())
			{	
				// ���ٸ� ������ �߰��ϰ� ����Ŭ Ȯ��
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
	// �˻� �� �ʱ�ȭ
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
	// �湮�� ��忡 ���°�� �湮�ߴ����� ���
	// -1�̸� ���� �湮���� ���� ���

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
		// ���� �湮�� ���� ���ٸ� �湮�Ѵ�.
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}

		if (_discoveredOrder[here] < _discoveredOrder[there])
			continue; // ������ ������ ���

		if (_finished[there] == false) // �������� �ƴѵ� DFS�� ������ ���� ���¶�� ������ ����
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
