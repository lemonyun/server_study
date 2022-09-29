#include "pch.h"
#include "JobQueue.h"
/*-----------------
	JobQueue
------------------*/

void JobQueue::Push(JobRef&& job)
{
	const int32 prevCount = _jobCount.fetch_add(1); // count ���� ��
	_jobs.Push(job); // ���������� WRITE_LOCK // job�� push (���ٰ� ������ �ٲ�� �ȵ�)

	// ù��° Job�� ���� �����尡 ������� ���
	if (prevCount == 0)
	{
		Execute();
	}
}

// 1) �ϰ��� �ʹ� ������?
// 2) DoAync Ÿ�� Ÿ�� ���� ���� ������ �ʴ� ��Ȳ

void JobQueue::Execute()
{
	while (true)
	{
		Vector<JobRef> jobs;
		_jobs.PopAll(OUT jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
			jobs[i]->Execute(); // job�� ������ ��
		
		// ���� �ϰ��� 0����� ����
		if (_jobCount.fetch_sub(jobCount) == jobCount) // count�� ���� 
		{
			return;
		}
	}
}
